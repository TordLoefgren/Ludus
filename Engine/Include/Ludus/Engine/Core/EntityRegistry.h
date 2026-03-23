#pragma once

#include <optional>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	struct EntityRegistry
	{
	private:
		std::vector<Entity> m_Data;										// Entity Storage.
		std::vector<EntityHandle> m_Handles;							// Index -> entity handle.
		std::unordered_map<EntityHandle, size_t> m_HandleToIndex;		// Entity handle -> index.
		Ludus::Engine::Core::Random m_Random;
		static constexpr size_t MaxUniqueIdAttempts = 32;

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);
				std::swap(m_Handles[index], m_Handles[lastIndex]);

				// Fix the indices of the moved element.
				const EntityHandle movedHandle = m_Handles[index];

				m_HandleToIndex[movedHandle] = index;
			}

			m_HandleToIndex.erase(m_Handles[lastIndex]);

			m_Data.pop_back();
			m_Handles.pop_back();
		}

		EntityHandle CommitEntity(Entity entity)
		{
			const auto handle = entity.Handle;
			LUDUS_ASSERT(!m_HandleToIndex.contains(handle), "Entity handle collision.");

			m_Data.push_back(std::move(entity));
			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = m_Data.size() - 1;

			return handle;
		}

		EntityHandle CreateUniqueId()
		{
			// Retry a fixed number of times to avoid collision loop.
			for (size_t attempt = 0; attempt < MaxUniqueIdAttempts; ++attempt)
			{
				const auto handle = m_Random.NextId();
				if (!m_HandleToIndex.contains(handle))
				{
					return handle;
				}
			}

			LUDUS_ASSERT(false, "Failed to generate a unique entity handle.");
			return m_Random.NextId();
		}

	public:
		void AddEntity(EntityHandle handle)
		{
			(void)CommitEntity(Entity { handle });
		}

		EntityHandle CreateEntity()
		{
			return CommitEntity(Entity { CreateUniqueId() });
		}

		bool DestroyEntity(EntityHandle handle)
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				RemoveAndReorderIndices(iter->second);

				return true;
			}

			return false;
		}

		Entity* TryGet(EntityHandle handle)
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				return &m_Data[iter->second];
			}

			return nullptr;
		}

		const Entity* TryGet(EntityHandle handle) const
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				return &m_Data[iter->second];
			}

			return nullptr;
		}

		std::span<const Entity> View() const { return { m_Data.data(), m_Data.size() }; }

		std::optional<size_t> IndexOf(EntityHandle handle) const
		{
			if (auto iter = m_HandleToIndex.find(handle); iter != m_HandleToIndex.end())
			{
				return iter->second;
			}

			return std::nullopt;
		}

		size_t GetCount() const { return m_Data.size(); }
	};
}
