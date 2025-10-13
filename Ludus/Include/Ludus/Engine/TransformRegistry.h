#pragma once

#include <cstdint>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/GameObject.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>

namespace Ludus::Engine
{
	struct TransformRegistry
	{
	private:
		std::vector<Ludus::Math::Transform2D> m_Data;								// Object Storage.
		std::vector<Ludus::Math::TransformHandle> m_Handles;						// Index -> transform handle.
		std::unordered_map<Ludus::Math::TransformHandle, size_t> m_HandleToIndex;	// Transform handle -> index.
		std::unordered_map<GameObjectHandle, size_t> m_OwnerHandleToIndex;			// Owner handle -> index.

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);
				std::swap(m_Handles[index], m_Handles[lastIndex]);

				// Fix the indices of the moved element.
				const Ludus::Math::TransformHandle movedHandle = m_Handles[index];
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
		std::span<const Ludus::Math::Transform2D> View() const { return { m_Data.data(), m_Data.size() }; }
		std::span<Ludus::Math::Transform2D> ViewMutable() { return { m_Data.data(), m_Data.size() }; }

		Ludus::Math::TransformHandle Add(
			GameObjectHandle owner,
			Ludus::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			m_Data.emplace_back(owner, position, scale, rotation);

			const auto index = m_Data.size() - 1;
			const auto handle = m_Data[index].Handle;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;
			m_OwnerHandleToIndex[owner] = index;

			return handle;
		}

		Ludus::Math::TransformHandle Add(Ludus::Math::Transform2D transform)
		{
			const auto owner = transform.OwnerHandle;
			const auto handle = transform.Handle;

			m_Data.push_back(std::move(transform));
			const auto index = m_Data.size() - 1;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;
			m_OwnerHandleToIndex[owner] = index;

			return handle;
		}

		bool RemoveById(Ludus::Math::TransformHandle handle)
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

		const Ludus::Math::Transform2D* TryGetById(Ludus::Math::TransformHandle handle) const
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		Ludus::Math::Transform2D* TryGetByIdMutable(Ludus::Math::TransformHandle handle)
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		const Ludus::Math::Transform2D* TryGetByOwner(GameObjectHandle ownerHandle) const
		{
			if (auto handleIter = m_OwnerHandleToIndex.find(ownerHandle); handleIter != m_OwnerHandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		Ludus::Math::Transform2D* TryGetByOwnerMutable(GameObjectHandle ownerHandle)
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
