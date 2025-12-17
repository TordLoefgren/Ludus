#pragma once

#include <concepts>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Core
{
	using Handle = uint32_t;

	template<typename T>
	struct ComponentRegistry
	{
	private:
		std::vector<T> m_Data;												// Object Storage.
		std::unordered_map<EntityHandle, size_t> m_OwnerHandleToIndex;		// Owner handle -> index.

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);

				// Fix the indices of the moved element.
				const EntityHandle movedOwnerHandle = m_Data[index].OwnerHandle;

				m_OwnerHandleToIndex[movedOwnerHandle] = index;
			}

			m_OwnerHandleToIndex.erase(m_Data[lastIndex].OwnerHandle);

			m_Data.pop_back();
		}

	public:
		std::span<const T> View() const { return { m_Data.data(), m_Data.size() }; }
		std::span<T> ViewMutable() { return { m_Data.data(), m_Data.size() }; }

		template<class... Args>
			requires std::constructible_from<T, EntityHandle, Args...>
		void Add(
			EntityHandle owner, Args&&... args
		)
		{
			LUDUS_ASSERT(!ContainsOwner(owner), "Invalid component handle.");

			m_Data.emplace_back(owner, std::forward<Args>(args)...);

			const auto index = m_Data.size() - 1;
			m_OwnerHandleToIndex[owner] = index;
		}

		void Add(T component)
		{
			const auto owner = component.OwnerHandle;

			LUDUS_ASSERT(!ContainsOwner(owner), "Invalid component handle.");

			m_Data.push_back(std::move(component));
			const auto index = m_Data.size() - 1;
			m_OwnerHandleToIndex[owner] = index;
		}

		bool RemoveByOwner(EntityHandle ownerHandle)
		{
			if (auto it = m_OwnerHandleToIndex.find(ownerHandle); it != m_OwnerHandleToIndex.end())
			{
				RemoveAndReorderIndices(it->second);
				return true;
			}

			return false;
		}

		const size_t GetCount() { return m_Data.size(); }

#pragma region Lookups

		const T* TryGetByOwner(EntityHandle ownerHandle) const
		{
			if (auto handleIter = m_OwnerHandleToIndex.find(ownerHandle); handleIter != m_OwnerHandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		T* TryGetByOwnerMutable(EntityHandle ownerHandle)
		{
			if (auto handleIter = m_OwnerHandleToIndex.find(ownerHandle); handleIter != m_OwnerHandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		bool ContainsOwner(EntityHandle ownerHandle) const { return m_OwnerHandleToIndex.contains(ownerHandle); }

#pragma endregion

	};
}
