#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	struct SceneRegistry
	{
	private:
		std::vector<Ludus::Engine::Core::Scene> m_Scenes;
		std::unordered_map<Ludus::Engine::Core::SceneHandle, size_t> m_HandleToIndex;
		Ludus::Engine::Core::Random m_Random;
		static constexpr size_t MaxUniqueIdAttempts = 32;

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

		Ludus::Engine::Core::SceneHandle CommitScene(Ludus::Engine::Core::Scene scene)
		{
			const auto sceneHandle = scene.Handle;
			LUDUS_ASSERT(!Contains(sceneHandle), "Scene handle collision.");

			m_Scenes.push_back(std::move(scene));
			m_HandleToIndex[sceneHandle] = m_Scenes.size() - 1;

			return sceneHandle;
		}

		Ludus::Engine::Core::SceneHandle CreateUniqueId()
		{
			// Retry a fixed number of times to avoid collision loop.
			for (size_t attempt = 0; attempt < MaxUniqueIdAttempts; ++attempt)
			{
				const auto handle = m_Random.NextId();
				if (!Contains(handle))
				{
					return handle;
				}
			}

			LUDUS_ASSERT(false, "Failed to generate a unique scene handle.");
			return 0;
		}

	public:
		SceneRegistry() = default;

		SceneRegistry(std::vector<Ludus::Engine::Core::Scene> scenes)
		{
			m_Scenes.reserve(scenes.size());
			m_HandleToIndex.reserve(scenes.size());

			for (auto& scene : scenes)
			{
				(void)CommitScene(std::move(scene));
			}
		}

		~SceneRegistry() = default;

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

		Ludus::Engine::Core::SceneHandle AddScene(std::string_view name)
		{
			return CommitScene(Ludus::Engine::Core::Scene { CreateUniqueId(), name });
		}

		Ludus::Engine::Core::SceneHandle AddScene(Ludus::Engine::Core::Scene scene)
		{
			return CommitScene(std::move(scene));
		}

		bool RemoveScene(Ludus::Engine::Core::SceneHandle handle)
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				RemoveAndReorderIndices(iter->second);

				return true;
			}

			return false;
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

		void Clear()
		{
			m_Scenes.clear();
			m_HandleToIndex.clear();
		}
	};
}
