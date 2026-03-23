#pragma once

#include <vector>

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Components
{
	struct Camera2DComponent;
	struct Collider2DComponent;
	struct DisplayNameComponent;
	struct RigidBody2DComponent;
	struct ScriptComponent;
	struct Sprite2DComponent;
	struct Text2DComponent;
	struct Transform2DComponent;
}

namespace Ludus::Engine::Runtime
{
	struct ScriptReference;
}

namespace Ludus::Editor::Panels
{
	class InspectorPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		static void DrawEntityHandle(Ludus::Engine::Core::EntityHandle handle);
		static bool DrawDisplayName(Ludus::Engine::Components::DisplayNameComponent& component);
		static bool DrawTransform2D(Ludus::Engine::Components::Transform2DComponent& component);
		static bool DrawCollider2D(Ludus::Engine::Components::Collider2DComponent& component);
		static bool DrawRigidBody2D(Ludus::Engine::Components::RigidBody2DComponent& component);
		static bool DrawScript(
			Ludus::Engine::Components::ScriptComponent& component,
			const std::vector<Ludus::Engine::Runtime::ScriptReference>& scriptReferences
		);
		static bool DrawSprite2D(Ludus::Engine::Components::Sprite2DComponent& component);
		static bool DrawText2D(Ludus::Engine::Components::Text2DComponent& component);
		static bool DrawCamera2D(Ludus::Engine::Components::Camera2DComponent& component);

	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Core::ProjectSessionContext& context) override { return &context.Shell.State.ActivePanelState.ShowInspectorPanel; }

		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;
	};
}
