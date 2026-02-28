#include "pch.h"

#include <Ludus/Editor/Core/SelectionManager.h>

namespace Ludus::Editor::Core
{
	void SelectionManager::SelectEntity(Ludus::Engine::Core::EntityHandle entity)
	{
		SelectedEntity = entity;
	}

	void SelectionManager::DeselectEntity(Ludus::Engine::Core::EntityHandle entity)
	{
		if (SelectedEntity == entity)
		{
			SelectedEntity = std::nullopt;
		}
	}

	void SelectionManager::ClearSelection()
	{
		SelectedEntity = std::nullopt;
	}

	bool SelectionManager::HasEntity() const
	{
		return SelectedEntity.has_value();
	}

	bool SelectionManager::IsSelected(Ludus::Engine::Core::EntityHandle entity) const
	{
		return SelectedEntity == entity;
	}
}
