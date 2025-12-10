#pragma once

#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Core
{
	struct EntityRegistry
	{
	private:
		std::vector<Entity> m_Data;										// Entity Storage.
		std::vector<EntityHandle> m_Handles;							// Index -> entity handle.
		std::unordered_map<EntityHandle, size_t> m_HandleToIndex;		// Entity handle -> index.

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

	public:

		EntityHandle CreateEntity()
		{
			m_Data.emplace_back();

			const auto index = m_Data.size() - 1;
			const auto handle = m_Data[index].Handle;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;

			return handle;
		}

		bool DestroyEntity(EntityHandle handle)
		{
			if (auto it = m_HandleToIndex.find(handle); it != m_HandleToIndex.end())
			{
				RemoveAndReorderIndices(it->second);

				return true;
			}

			return false;
		}

		Entity* TryGet(EntityHandle handle)
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		const Entity* TryGet(EntityHandle handle) const
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		const size_t GetCount() { return m_Data.size(); }
	};
}
