#pragma once

#include <optional>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Editor::Core
{
	struct SelectionManager
	{
		std::optional<Ludus::Engine::Core::EntityHandle> SelectedEntity;

		void SelectEntity(Ludus::Engine::Core::EntityHandle entity);
		void DeselectEntity(Ludus::Engine::Core::EntityHandle entity);

		void ClearSelection();

		bool HasEntity() const;
		bool IsSelected(Ludus::Engine::Core::EntityHandle entity) const;
	};
}
