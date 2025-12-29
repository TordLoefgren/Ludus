#pragma once

#include <optional>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Core
{
	struct EditorSelection
	{
		std::optional<Ludus::Engine::Core::EntityHandle> SelectedEntity;
		std::optional<Ludus::Engine::Core::SceneHandle> SelectedScene;

		void SelectEntity(Ludus::Engine::Core::EntityHandle entity, Ludus::Engine::Core::SceneHandle scene)
		{
			SelectedEntity = entity;
			SelectedScene = scene;
		}

		void SelectScene(Ludus::Engine::Core::SceneHandle scene)
		{
			SelectedScene = scene;
			SelectedEntity = std::nullopt;
		}

		void DeselectEntity()
		{
			SelectedEntity = std::nullopt;
		}

		void DeselectScene()
		{
			SelectedScene = std::nullopt;
			SelectedEntity = std::nullopt;
		}

		bool HasEntity() const { return SelectedEntity.has_value(); }

		bool HasScene() const { return SelectedScene.has_value(); }

		bool IsSelected(Ludus::Engine::Core::EntityHandle entity, Ludus::Engine::Core::SceneHandle scene) const
		{
			return SelectedEntity == entity && SelectedScene == scene;
		}
	};
}
