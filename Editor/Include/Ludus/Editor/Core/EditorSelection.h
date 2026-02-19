#pragma once

#include <optional>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Core
{
	struct EditorSelection
	{
		std::optional<Ludus::Engine::Core::EntityHandle> SelectedEntity;

		void SelectEntity(const Ludus::Engine::Core::EntityHandle entity)
		{
			SelectedEntity = entity;
		}

		void DeselectEntity()
		{
			SelectedEntity = std::nullopt;
		}

		bool HasEntity() const
		{
			return SelectedEntity.has_value();
		}

		bool IsSelected(const Ludus::Engine::Core::EntityHandle entity) const
		{
			return SelectedEntity == entity;
		}
	};
}
