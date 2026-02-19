#pragma once

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Core/EditorSceneMetadata.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Core
{
	struct EditorSession
	{
	private:
		std::unordered_map<Ludus::Engine::Core::SceneHandle, EditorSceneMetadata> m_SceneMetadata;
		std::optional<Ludus::Engine::Core::SceneHandle> m_ActiveScene;

	public:
		bool HasMetadata(Ludus::Engine::Core::SceneHandle handle) const
		{
			return m_SceneMetadata.contains(handle);
		}

		EditorSceneMetadata* TryGetMetadata(Ludus::Engine::Core::SceneHandle handle)
		{
			auto iter = m_SceneMetadata.find(handle);
			return iter == m_SceneMetadata.end() ? nullptr : &iter->second;
		}

		const EditorSceneMetadata* TryGetMetadata(Ludus::Engine::Core::SceneHandle handle) const
		{
			auto iter = m_SceneMetadata.find(handle);
			return iter == m_SceneMetadata.end() ? nullptr : &iter->second;
		}

		EditorSceneMetadata& GetMetadata(Ludus::Engine::Core::SceneHandle handle)
		{
			auto* metadata = TryGetMetadata(handle);
			LUDUS_ASSERT(metadata != nullptr, "Missing editor metadata for scene.");
			return *metadata;
		}

		void EnsureMetadata(Ludus::Engine::Core::SceneHandle handle, EditorSceneMetadata metadata = {})
		{
			m_SceneMetadata.try_emplace(handle, std::move(metadata));
			if (!m_ActiveScene.has_value())
			{
				m_ActiveScene = handle;
			}
		}

		bool RemoveMetadata(Ludus::Engine::Core::SceneHandle handle)
		{
			const bool wasActive = m_ActiveScene && *m_ActiveScene == handle;

			const auto removed = m_SceneMetadata.erase(handle) > 0;
			if (!removed)
			{
				return false;
			}

			if (wasActive)
			{
				m_ActiveScene.reset();

				// Choose a new active scene if there are any remaining.
				if (!m_SceneMetadata.empty())
				{
					m_ActiveScene = m_SceneMetadata.begin()->first;
				}
			}

			return true;
		}

		void SetActiveScene(Ludus::Engine::Core::SceneHandle handle)
		{
			LUDUS_ASSERT(HasMetadata(handle), "Active editor scene must have metadata.");
			m_ActiveScene = handle;
		}

		std::optional<Ludus::Engine::Core::SceneHandle> GetActiveScene() const { return m_ActiveScene; }

		bool IsDirty(Ludus::Engine::Core::SceneHandle handle) const
		{
			if (auto* metadata = TryGetMetadata(handle))
			{
				return metadata->IsDirty;
			}

			return false;
		}

		void MarkDirty(Ludus::Engine::Core::SceneHandle handle, bool dirty = true)
		{
			auto& metadata = GetMetadata(handle);
			metadata.IsDirty = dirty;
		}

		bool IsSaved(Ludus::Engine::Core::SceneHandle handle) const
		{
			if (auto* m = TryGetMetadata(handle))
			{
				return m->Path.has_value();
			}

			return false;
		}

		void SetPath(Ludus::Engine::Core::SceneHandle handle, std::filesystem::path path)
		{
			auto& metadata = GetMetadata(handle);
			metadata.Path = std::move(path);
		}

		std::vector<Ludus::Engine::Core::SceneHandle> GetOpenInHierarchyHandles() const
		{
			std::vector<Ludus::Engine::Core::SceneHandle> result;
			result.reserve(m_SceneMetadata.size());

			for (const auto& [handle, metadata] : m_SceneMetadata)
			{
				if (metadata.IsOpenInHierarchy)
				{
					result.push_back(handle);
				}
			}

			return result;
		}

		void Clear()
		{
			m_SceneMetadata.clear();
			m_ActiveScene.reset();
		}
	};
}
