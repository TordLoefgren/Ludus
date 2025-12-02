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

			if (!(transformPtr && colliderPtr && rigidBodyPtr && spritePtr && textPtr))
			{
				return;
			}

			auto& transform = *transformPtr;
			auto& collider = *colliderPtr;
			auto& rigidBody = *rigidBodyPtr;
			auto& sprite = *spritePtr;
			auto& text = *textPtr;

			Ludus::Editor::Core::EntityPanel(handle);
			Ludus::Editor::Core::Transform2DPanel(transform);
			Ludus::Editor::Core::Collider2DPanel(collider);
			Ludus::Editor::Core::RigidBody2DPanel(rigidBody);
			Ludus::Editor::Core::Sprite2DPanel(sprite);
			Ludus::Editor::Core::Text2DPanel(text);
		}
	}
}
