#include "pch.h"

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/InspectorHelpers.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>

namespace Ludus::Editor::Panels
{
	void InspectorPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		auto windowTitle = CreateWindowTitle("Inspector");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& selection = context.EditorContext.State.Selection;
			if (!selection.HasScene() || !selection.HasEntity())
			{
				return;
			}

			auto* scene = context.SystemContext.SceneManager.TryGetScene(selection.SelectedScene.value());

			if (!scene)
			{
				return;
			}

			auto& ecs = scene->EntityComponentSystem;
			auto handle = selection.SelectedEntity.value();

			auto* transformPtr = ecs.Transforms.TryGetByOwnerMutable(handle);
			auto* colliderPtr = ecs.Colliders.TryGetByOwnerMutable(handle);
			auto* rigidBodyPtr = ecs.RigidBodies.TryGetByOwnerMutable(handle);
			auto* spritePtr = ecs.Sprites.TryGetByOwnerMutable(handle);
			auto* textPtr = ecs.Texts.TryGetByOwnerMutable(handle);
			auto* cameraPtr = ecs.Cameras.TryGetByOwnerMutable(handle);

			Ludus::Editor::Core::EntityPanel(handle);

			if (transformPtr)
			{
				Ludus::Editor::Core::Transform2DPanel(*transformPtr);
			}

			if (colliderPtr)
			{
				Ludus::Editor::Core::Collider2DPanel(*colliderPtr);
			}

			if (rigidBodyPtr)
			{
				Ludus::Editor::Core::RigidBody2DPanel(*rigidBodyPtr);
			}

			if (spritePtr)
			{
				Ludus::Editor::Core::Sprite2DPanel(*spritePtr);
			}

			if (textPtr)
			{
				Ludus::Editor::Core::Text2DPanel(*textPtr);
			}

			if (cameraPtr)
			{
				Ludus::Editor::Core::Camera2DPanel(*cameraPtr);
			}
		}
	}
}
