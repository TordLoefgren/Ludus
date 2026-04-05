#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	struct SceneRegistry
	{
	private:
		std::vector<Ludus::Engine::Core::Scene> m_Scenes;
		std::unordered_map<Ludus::Engine::Core::SceneId, size_t> m_IdToIndex;
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
				const auto movedId = m_Scenes[index].Id;
				m_IdToIndex[movedId] = index;
			}

			const auto removedId = m_Scenes[lastIndex].Id;
			m_IdToIndex.erase(removedId);

			m_Scenes.pop_back();
		}

		Ludus::Engine::Core::SceneId CommitScene(Ludus::Engine::Core::Scene scene)
		{
			auto sceneId = scene.Id;
			if (!sceneId.IsValid())
			{
				sceneId = CreateUniqueId();
			}
			else
			{
				LUDUS_ASSERT(!Contains(sceneId), "Cannot add a scene with a duplicate id.");
			}

			scene.Id = sceneId;

			m_Scenes.push_back(std::move(scene));
			m_IdToIndex[sceneId] = m_Scenes.size() - 1;

			return sceneId;
		}

		Ludus::Engine::Core::SceneId CreateUniqueId()
		{
			// Retry a fixed number of times to avoid collision loop.
			for (size_t attempt = 0; attempt < MaxUniqueIdAttempts; ++attempt)
			{
				const auto id = SceneId { m_Random.NextId() };
				if (!Contains(id))
				{
					return id;
				}
			}

			LUDUS_ASSERT(false, "Failed to generate a unique scene id.");
			return SceneId::Invalid();
		}

	public:
		SceneRegistry() = default;

		SceneRegistry(std::vector<Ludus::Engine::Core::Scene> scenes)
		{
			m_Scenes.reserve(scenes.size());
			m_IdToIndex.reserve(scenes.size());

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

		bool Contains(Ludus::Engine::Core::SceneId id) const
		{
			return m_IdToIndex.contains(id);
		}

		Ludus::Engine::Core::SceneId AddScene(std::string_view name)
		{
			return CommitScene(Ludus::Engine::Core::Scene { SceneId::Invalid(), name });
		}

		Ludus::Engine::Core::SceneId AddScene(Ludus::Engine::Core::Scene scene)
		{
			return CommitScene(std::move(scene));
		}

		bool RemoveScene(Ludus::Engine::Core::SceneId id)
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				RemoveAndReorderIndices(iter->second);

				return true;
			}

			return false;
		}

		Ludus::Engine::Core::Scene* TryGetScene(Ludus::Engine::Core::SceneId id)
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				return &m_Scenes[iter->second];
			}

			return nullptr;
		}

		const Ludus::Engine::Core::Scene* TryGetScene(Ludus::Engine::Core::SceneId id) const
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				return &m_Scenes[iter->second];
			}

			return nullptr;
		}

		Ludus::Engine::Core::Scene& GetScene(Ludus::Engine::Core::SceneId id)
		{
			auto* scene = TryGetScene(id);
			LUDUS_ASSERT(scene != nullptr, "Scene id does not exist.");
			return *scene;
		}

		const Ludus::Engine::Core::Scene& GetScene(Ludus::Engine::Core::SceneId id) const
		{
			const auto* scene = TryGetScene(id);
			LUDUS_ASSERT(scene != nullptr, "Scene id does not exist.");
			return *scene;
		}

		void Clear()
		{
			m_Scenes.clear();
			m_IdToIndex.clear();
		}
	};
}
