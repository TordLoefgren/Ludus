#pragma once

#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/GameObject.h>

namespace Ludus::Engine
{
	using Ludus::Engine::GameObject;
	using Ludus::Engine::GameObjectHandle;

	struct GameObjectRegistry
	{
	private:
		std::vector<GameObject> m_Data;											// Object Storage.
		std::vector<GameObjectHandle> m_Handles;								// Index -> game object handle.
		std::unordered_map<GameObjectHandle, size_t> m_HandleToIndex;			// Game object handle -> index.

		void RemoveAndReorderIndices(size_t index)
		{
			const size_t lastIndex = m_Data.size() - 1;
			if (index != lastIndex)
			{
				std::swap(m_Data[index], m_Data[lastIndex]);
				std::swap(m_Handles[index], m_Handles[lastIndex]);

				// Fix the indices of the moved element.
				const GameObjectHandle movedHandle = m_Handles[index];

				m_HandleToIndex[movedHandle] = index;
			}

			m_HandleToIndex.erase(m_Handles[lastIndex]);

			m_Data.pop_back();
			m_Handles.pop_back();
		}

	public:

		GameObjectHandle CreateGameObject()
		{
			auto gameObject = GameObject();
			m_Data.push_back(std::move(gameObject));

			const auto index = m_Data.size() - 1;
			const auto handle = m_Data[index].Handle;

			m_Handles.push_back(handle);
			m_HandleToIndex[handle] = index;

			return handle;
		}

		bool DestroyGameObject(GameObjectHandle handle)
		{
			if (auto it = m_HandleToIndex.find(handle); it != m_HandleToIndex.end())
			{
				RemoveAndReorderIndices(it->second);
				return true;
			}

			return false;
		}

		GameObject* TryGet(GameObjectHandle handle)
		{
			if (auto handleIter = m_HandleToIndex.find(handle); handleIter != m_HandleToIndex.end())
			{
				return &m_Data[handleIter->second];
			}

			return nullptr;
		}

		const GameObject* TryGet(GameObjectHandle handle) const
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
