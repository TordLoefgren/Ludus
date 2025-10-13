#pragma once

#include <cstdint>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Physics/Collider2D.h>

namespace Ludus::Engine
{
	struct ColliderRegistry
	{
	private:
		std::vector<Ludus::Physics::Collider2D> m_Data;								// Object Storage.
		std::vector<Ludus::Physics::ColliderHandle> m_Handles;						// Index -> collider handle.
		std::unordered_map<Ludus::Physics::ColliderHandle, size_t> m_HandleToIndex;	// Collider handle -> index.
		std::unordered_map<GameObjectHandle, size_t> m_OwnerHandleToIndex;			// Owner handle -> index.

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);
				std::swap(m_Handles[index], m_Handles[lastIndex]);

				// Fix the indices of the moved element.
				const Ludus::Physics::ColliderHandle movedHandle = m_Handles[index];
				const GameObjectHandle movedOwnerHandle = m_Data[index].OwnerHandle;

				m_HandleToIndex[movedHandle] = index;
				m_OwnerHandleToIndex[movedOwnerHandle] = index;
			}

			m_HandleToIndex.erase(m_Handles[lastIndex]);
			m_OwnerHandleToIndex.erase(m_Data[lastIndex].OwnerHandle);

			m_Data.pop_back();
			m_Handles.pop_back();
		}

	public:
		std::span<const Ludus::Physics::Collider2D> View() const { return { m_Data.data(), m_Data.size() }; }
		std::span<Ludus::Physics::Collider2D> ViewMutable() { return { m_Data.data(), m_Data.size() }; }

		Ludus::Physics::ColliderHandle Add(
			GameObjectHandle owner,
			Ludus::Physics::Index layer,
			LayerMask collidesWith = LayerMask::GetEmpty(),
			bool isStatic = false
		)
		{
			m_Data.emplace_back(owner, layer, collidesWith, isStatic);

			const auto index = m_Data.size() - 1;
			const auto handle = m_Data[index].Handle;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;
			m_OwnerHandleToIndex[owner] = index;

			return handle;
		}

		Ludus::Physics::ColliderHandle Add(Ludus::Physics::Collider2D collider)
		{
			const auto owner = collider.OwnerHandle;
			const auto handle = collider.Handle;

			m_Data.push_back(std::move(collider));
			const auto index = m_Data.size() - 1;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;
			m_OwnerHandleToIndex[owner] = index;

			return handle;
		}

		bool RemoveById(Ludus::Physics::ColliderHandle handle)
		{
			if (auto it = m_HandleToIndex.find(handle); it != m_HandleToIndex.end())
			{
				RemoveAndReorderIndices(it->second);
				return true;
			}

			return false;
		}

		bool RemoveByOwner(GameObjectHandle ownerHandle)
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

		const Ludus::Physics::Collider2D* TryGetById(Ludus::Physics::ColliderHandle handle) const
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		Ludus::Physics::Collider2D* TryGetByIdMutable(Ludus::Physics::ColliderHandle handle)
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		const Ludus::Physics::Collider2D* TryGetByOwner(GameObjectHandle ownerHandle) const
		{
			if (auto handleIter = m_OwnerHandleToIndex.find(ownerHandle); handleIter != m_OwnerHandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		Ludus::Physics::Collider2D* TryGetByOwnerMutable(GameObjectHandle ownerHandle)
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
