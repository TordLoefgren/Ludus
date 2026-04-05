#pragma once

#include <concepts>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Core
{
	template<typename T>
	struct ComponentRegistry
	{
	private:
		std::vector<T> m_Data;										// Object Storage.
		std::unordered_map<EntityId, size_t> m_OwnerIdToIndex;		// Owner id -> index.

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);

				// Fix the indices of the moved element.
				const EntityId movedOwnerId = m_Data[index].OwnerId;

				m_OwnerIdToIndex[movedOwnerId] = index;
			}

			m_OwnerIdToIndex.erase(m_Data[lastIndex].OwnerId);

			m_Data.pop_back();
		}

	public:
		std::span<const T> View() const { return { m_Data.data(), m_Data.size() }; }
		std::span<T> ViewMutable() { return { m_Data.data(), m_Data.size() }; }

		template<typename... Args>
			requires std::constructible_from<T, EntityId, Args...>
		void Add(EntityId owner, Args&&... args)
		{
			LUDUS_ASSERT(!ContainsOwner(owner), "Invalid component handle.");

			m_Data.emplace_back(owner, std::forward<Args>(args)...);

			const auto index = m_Data.size() - 1;
			m_OwnerIdToIndex[owner] = index;
		}

		void Add(T component)
		{
			const auto owner = component.OwnerId;

			LUDUS_ASSERT(!ContainsOwner(owner), "Invalid component handle.");

			m_Data.push_back(std::move(component));
			const auto index = m_Data.size() - 1;
			m_OwnerIdToIndex[owner] = index;
		}

		bool RemoveByOwner(EntityId ownerId)
		{
			if (auto iter = m_OwnerIdToIndex.find(ownerId); iter != m_OwnerIdToIndex.end())
			{
				RemoveAndReorderIndices(iter->second);
				return true;
			}

			return false;
		}

		const size_t GetCount() { return m_Data.size(); }

#pragma region Lookups

		const T* TryGetByOwner(EntityId ownerId) const
		{
			if (auto iter = m_OwnerIdToIndex.find(ownerId); iter != m_OwnerIdToIndex.end())
			{
				return &m_Data[iter->second];
			}

			return nullptr;
		}

		T* TryGetByOwnerMutable(EntityId ownerId)
		{
			if (auto iter = m_OwnerIdToIndex.find(ownerId); iter != m_OwnerIdToIndex.end())
			{
				return &m_Data[iter->second];
			}

			return nullptr;
		}

		bool ContainsOwner(EntityId ownerId) const { return m_OwnerIdToIndex.contains(ownerId); }

#pragma endregion

	};
}
