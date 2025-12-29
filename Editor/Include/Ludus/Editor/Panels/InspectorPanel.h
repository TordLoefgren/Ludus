#pragma once

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Editor::Panels
{
	class InspectorPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		static void DrawEntityHandle(Ludus::Engine::Core::EntityHandle handle);
		static void DrawDisplayName(Ludus::Engine::Components::DisplayNameComponent& component);
		static void DrawTransform2D(Ludus::Engine::Components::Transform2DComponent& component);
		static void DrawCollider2D(Ludus::Engine::Components::Collider2DComponent& component);
		static void DrawRigidBody2D(Ludus::Engine::Components::RigidBody2DComponent& component);
		static void DrawSprite2D(Ludus::Engine::Components::Sprite2DComponent& component);
		static void DrawText2D(Ludus::Engine::Components::Text2DComponent& component);
		static void DrawCamera2D(Ludus::Engine::Components::Camera2DComponent& component);

	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Panels::PanelContext& context) override { return &context.ActivePanelState.ShowInspectorPanel; }

		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
