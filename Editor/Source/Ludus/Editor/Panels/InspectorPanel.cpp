#include "Ludus/Editor/Panels/InspectorPanel.h"

namespace Ludus::Editor::Panels
{
	void InspectorPanel::DrawPanel()
	{
		static bool open = true;
		if (Ludus::UI::Containers::Window window("Inspector", &open, Constants::PanelFlags); window)
		{
			// Inspector panel test.
			auto& ecs = m_SystemContext->EntityComponentSystem;

			auto handle = 1;

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
