#pragma once

#pragma once

#include <concepts>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Entity.h>

namespace Ludus::Engine
{
	using Handle = uint32_t;

	template<typename T>
	struct ComponentRegistry
	{
	private:
		std::vector<T> m_Data;												// Object Storage.
		std::vector<Handle> m_Handles;										// Index -> component handle.
		std::unordered_map<Handle, size_t> m_HandleToIndex;					// Collider handle -> index.
		std::unordered_map<EntityHandle, size_t> m_OwnerHandleToIndex;		// Owner handle -> index.

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);
				std::swap(m_Handles[index], m_Handles[lastIndex]);

				// Fix the indices of the moved element.
				const Handle movedHandle = m_Handles[index];
				const EntityHandle movedOwnerHandle = m_Data[index].OwnerHandle;

				m_HandleToIndex[movedHandle] = index;
				m_OwnerHandleToIndex[movedOwnerHandle] = index;
			}

			m_HandleToIndex.erase(m_Handles[lastIndex]);
			m_OwnerHandleToIndex.erase(m_Data[lastIndex].OwnerHandle);

			m_Data.pop_back();
			m_Handles.pop_back();
		}

	public:
		std::span<const T> View() const { return { m_Data.data(), m_Data.size() }; }
		std::span<T> ViewMutable() { return { m_Data.data(), m_Data.size() }; }

		template<class... Args>
			requires std::constructible_from<T, EntityHandle, Args...>
		Handle Add(
			EntityHandle owner, Args&&... args
		)
		{
			m_Data.emplace_back(owner, std::forward<Args>(args)...);

			const auto index = m_Data.size() - 1;
			const auto handle = m_Data[index].Handle;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;
			m_OwnerHandleToIndex[owner] = index;

			return handle;
		}

		Handle Add(T component)
		{
			const auto owner = component.OwnerHandle;
			const auto handle = component.Handle;

			m_Data.push_back(std::move(component));
			const auto index = m_Data.size() - 1;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;
			m_OwnerHandleToIndex[owner] = index;

			return handle;
		}

		bool RemoveById(Handle handle)
		{
			if (auto it = m_HandleToIndex.find(handle); it != m_HandleToIndex.end())
			{
				RemoveAndReorderIndices(it->second);
				return true;
			}

			return false;
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

		const T* TryGetById(Handle handle) const
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		T* TryGetByIdMutable(Handle handle)
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

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

#pragma endregion

	};
}
