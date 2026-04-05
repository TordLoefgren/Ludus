#include "pch.h"

#include <Ludus/Editor/Core/SelectionManager.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Core
{
	void SelectionManager::SelectEntity(Ludus::Engine::Core::EntityId id)
	{
		SelectedEntityId = id;
	}

	void SelectionManager::DeselectEntity(Ludus::Engine::Core::EntityId id)
	{
		if (SelectedEntityId == id)
		{
			SelectedEntityId = std::nullopt;
		}
	}

	void SelectionManager::ClearSelection()
	{
		SelectedEntityId = std::nullopt;
	}

	bool SelectionManager::HasEntity() const
	{
		return SelectedEntityId.has_value();
	}

	bool SelectionManager::IsSelected(Ludus::Engine::Core::EntityId id) const
	{
		return SelectedEntityId == id;
	}
}
