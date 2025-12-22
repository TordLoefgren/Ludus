#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	struct SceneManager
	{
	private:
		std::vector<Ludus::Engine::Core::Scene> m_Scenes;
		std::unordered_map<Ludus::Engine::Core::SceneHandle, size_t> m_HandleToIndex;

		std::optional<Ludus::Engine::Core::SceneHandle> m_ActiveSceneHandle;

	private:
		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Scenes.size() - 1;

			if (index != lastIndex)
			{
				std::swap(m_Scenes[index], m_Scenes[lastIndex]);

				// Fix the indices of the moved scene.
				const auto movedHandle = m_Scenes[index].Handle;
				m_HandleToIndex[movedHandle] = index;
			}

			const auto removedHandle = m_Scenes[lastIndex].Handle;
			m_HandleToIndex.erase(removedHandle);

			m_Scenes.pop_back();
		}

	public:
		SceneManager() = default;

		SceneManager(std::vector<Ludus::Engine::Core::Scene> scenes)
			: m_Scenes(std::move(scenes))
		{
			m_HandleToIndex.reserve(m_Scenes.size());
			for (size_t i = 0; i < m_Scenes.size(); ++i)
			{
				m_HandleToIndex[m_Scenes[i].Handle] = i;
			}

			if (!m_Scenes.empty())
			{
				m_ActiveSceneHandle = m_Scenes.front().Handle;
			}
		}

		~SceneManager() = default;

	public:
		std::span<const Ludus::Engine::Core::Scene> View() const
		{
			return { m_Scenes.data(), m_Scenes.size() };
		}

		std::span<Ludus::Engine::Core::Scene> ViewMutable()
		{
			return { m_Scenes.data(), m_Scenes.size() };
		}

		bool Contains(Ludus::Engine::Core::SceneHandle handle) const
		{
			return m_HandleToIndex.contains(handle);
		}

		Ludus::Engine::Core::SceneHandle AddScene()
		{
			m_Scenes.emplace_back();
			auto& scene = m_Scenes.back();

			LUDUS_ASSERT(!Contains(scene.Handle), "Scene handle collision.");

			const auto index = m_Scenes.size() - 1;

			const auto handle = m_Scenes[index].Handle;
			m_HandleToIndex[handle] = index;

			// If this is the first scene, make it the active scene.
			if (!m_ActiveSceneHandle.has_value())
			{
				m_ActiveSceneHandle = handle;
			}

			return handle;
		}

		bool RemoveScene(Ludus::Engine::Core::SceneHandle handle)
		{
			if (auto it = m_HandleToIndex.find(handle); it != m_HandleToIndex.end())
			{
				const bool removedWasActive = m_ActiveSceneHandle.has_value() && m_ActiveSceneHandle.value() == handle;

				RemoveAndReorderIndices(it->second);

				if (removedWasActive)
				{
					if (m_Scenes.empty())
					{
						m_ActiveSceneHandle.reset();
					}
					else
					{
						// Pick the first scene from the collection as the new active scene.
						m_ActiveSceneHandle = m_Scenes.front().Handle;
					}
				}

				return true;
			}

			return false;
		}

		void SetActiveScene(Ludus::Engine::Core::SceneHandle handle)
		{
			LUDUS_ASSERT(Contains(handle), "Attempted to set active scene to a handle that does not exist.");
			m_ActiveSceneHandle = handle;
		}

		std::optional<Ludus::Engine::Core::SceneHandle> GetActiveSceneHandle() const
		{
			return m_ActiveSceneHandle;
		}

		Ludus::Engine::Core::Scene* TryGetScene(Ludus::Engine::Core::SceneHandle handle)
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				return &m_Scenes[iter->second];
			}

			return nullptr;
		}

		const Ludus::Engine::Core::Scene* TryGetScene(Ludus::Engine::Core::SceneHandle handle) const
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				return &m_Scenes[iter->second];
			}

			return nullptr;
		}

		Ludus::Engine::Core::Scene& GetScene(Ludus::Engine::Core::SceneHandle handle)
		{
			auto* scene = TryGetScene(handle);
			LUDUS_ASSERT(scene != nullptr, "Scene handle does not exist.");
			return *scene;
		}

		const Ludus::Engine::Core::Scene& GetScene(Ludus::Engine::Core::SceneHandle handle) const
		{
			const auto* scene = TryGetScene(handle);
			LUDUS_ASSERT(scene != nullptr, "Scene handle does not exist.");
			return *scene;
		}

		Ludus::Engine::Core::Scene* TryGetActiveScene()
		{
			if (!m_ActiveSceneHandle.has_value())
			{
				return nullptr;
			}

			return TryGetScene(m_ActiveSceneHandle.value());
		}

		const Ludus::Engine::Core::Scene* TryGetActiveScene() const
		{
			if (!m_ActiveSceneHandle.has_value())
			{
				return nullptr;
			}

			return TryGetScene(m_ActiveSceneHandle.value());
		}

		Ludus::Engine::Core::Scene& GetActiveScene()
		{
			auto* scene = TryGetActiveScene();
			LUDUS_ASSERT(scene != nullptr, "No active scene is set.");
			return *scene;
		}

		const Ludus::Engine::Core::Scene& GetActiveScene() const
		{
			const auto* scene = TryGetActiveScene();
			LUDUS_ASSERT(scene != nullptr, "No active scene is set.");
			return *scene;
		}
	};
}
