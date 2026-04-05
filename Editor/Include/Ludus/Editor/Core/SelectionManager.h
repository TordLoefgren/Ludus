#pragma once

#include <optional>

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Core
{
	struct SelectionManager
	{
		std::optional<Ludus::Engine::Core::EntityId> SelectedEntityId;

		void SelectEntity(Ludus::Engine::Core::EntityId id);
		void DeselectEntity(Ludus::Engine::Core::EntityId id);

		void ClearSelection();

		bool HasEntity() const;
		bool IsSelected(Ludus::Engine::Core::EntityId id) const;
	};
}
