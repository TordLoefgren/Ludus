#pragma once

#include <optional>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	struct EntityRegistry
	{
	private:
		std::vector<Entity> m_Data;									// Entity Storage.
		std::vector<EntityId> m_Ids;								// Index -> entity id.
		std::unordered_map<EntityId, size_t> m_IdToIndex;			// Entity id -> index.
		Ludus::Engine::Core::Random m_Random;
		static constexpr size_t MaxUniqueIdAttempts = 32;

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);
				std::swap(m_Ids[index], m_Ids[lastIndex]);

				// Fix the indices of the moved element.
				const EntityId movedId = m_Ids[index];

				m_IdToIndex[movedId] = index;
			}

			m_IdToIndex.erase(m_Ids[lastIndex]);

			m_Data.pop_back();
			m_Ids.pop_back();
		}

		EntityId CommitEntity(Entity entity)
		{
			const auto id = CreateUniqueId();
			entity.Id = id;

			m_Data.push_back(std::move(entity));
			m_Ids.push_back(id);
			m_IdToIndex[id] = m_Data.size() - 1;

			return id;
		}

		EntityId CreateUniqueId()
		{
			// Retry a fixed number of times to avoid collision loop.
			for (size_t attempt = 0; attempt < MaxUniqueIdAttempts; ++attempt)
			{
				const auto id = EntityId { m_Random.NextId() };
				if (!m_IdToIndex.contains(id))
				{
					return id;
				}
			}

			LUDUS_ASSERT(false, "Failed to generate a unique entity id.");
			return EntityId { m_Random.NextId() };
		}

	public:
		EntityId AddEntity(Entity entity)
		{
			return CommitEntity(std::move(entity));
		}

		EntityId CreateEntity()
		{
			return CommitEntity(Entity { EntityId::Invalid() });
		}

		EntityId RestoreEntity(EntityId id)
		{
			LUDUS_ASSERT(id.IsValid(), "Cannot restore an invalid entity id.");
			LUDUS_ASSERT(!m_IdToIndex.contains(id), "Cannot restore a duplicate entity id.");

			auto entity = Entity { id };

			m_Data.push_back(entity);
			m_Ids.push_back(id);
			m_IdToIndex[id] = m_Data.size() - 1;

			return id;
		}

		bool DestroyEntity(EntityId id)
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				RemoveAndReorderIndices(iter->second);

				return true;
			}

			return false;
		}

		Entity* TryGet(EntityId id)
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				return &m_Data[iter->second];
			}

			return nullptr;
		}

		const Entity* TryGet(EntityId id) const
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				return &m_Data[iter->second];
			}

			return nullptr;
		}

		std::span<const Entity> View() const { return { m_Data.data(), m_Data.size() }; }

		std::optional<size_t> IndexOf(EntityId id) const
		{
			if (auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				return iter->second;
			}

			return std::nullopt;
		}

		size_t GetCount() const { return m_Data.size(); }
	};
}
