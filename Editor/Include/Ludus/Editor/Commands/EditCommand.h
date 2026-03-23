#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <Ludus/Editor/Commands/EntityReference.h>
#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;

	struct EditCommand
	{

#pragma region Component Commands

		using EntityHandle = Ludus::Engine::Core::EntityHandle;
		using SceneHandle = Ludus::Engine::Core::SceneHandle;
		using Camera2DComponent = Ludus::Engine::Components::Camera2DComponent;
		using Collider2DComponent = Ludus::Engine::Components::Collider2DComponent;
		using DisplayNameComponent = Ludus::Engine::Components::DisplayNameComponent;
		using RigidBody2DComponent = Ludus::Engine::Components::RigidBody2DComponent;
		using ScriptComponent = Ludus::Engine::Components::ScriptComponent;
		using Sprite2DComponent = Ludus::Engine::Components::Sprite2DComponent;
		using Text2DComponent = Ludus::Engine::Components::Text2DComponent;
		using Transform2DComponent = Ludus::Engine::Components::Transform2DComponent;

		struct UseDefault { };

		template<typename TComponent>
		struct AddComponent { EntityReference EntityReference { }; SceneHandle SceneHandle; std::variant<UseDefault, TComponent> Init { UseDefault { } }; };

		template<typename TComponent>
		struct RemoveComponent { EntityReference EntityReference; SceneHandle SceneHandle; };

		template<typename TComponent>
		struct UpdateComponent { EntityReference EntityReference; SceneHandle SceneHandle; TComponent Before; TComponent After; };

#pragma endregion

#pragma region Entity Commands

		struct AddEntity { EntityReference EntityReference; SceneHandle SceneHandle; };
		struct RemoveEntity { EntityReference EntityReference; SceneHandle SceneHandle; };

#pragma endregion

#pragma region Selection Commands

		struct SelectEntity { EntityReference EntityReference; };
		struct DeselectEntity { EntityReference EntityReference; };
		struct ClearSelection { };

#pragma endregion

		using Variant = std::variant<
			AddComponent<Camera2DComponent>, AddComponent<Collider2DComponent>, AddComponent<DisplayNameComponent>, AddComponent<RigidBody2DComponent>,
			AddComponent<ScriptComponent>, AddComponent<Sprite2DComponent>, AddComponent<Text2DComponent>, AddComponent<Transform2DComponent>,

			RemoveComponent<Camera2DComponent>, RemoveComponent<Collider2DComponent>, RemoveComponent<DisplayNameComponent>, RemoveComponent<RigidBody2DComponent>,
			RemoveComponent<ScriptComponent>, RemoveComponent<Sprite2DComponent>, RemoveComponent<Text2DComponent>, RemoveComponent<Transform2DComponent>,

			UpdateComponent<Camera2DComponent>, UpdateComponent<Collider2DComponent>, UpdateComponent<DisplayNameComponent>, UpdateComponent<RigidBody2DComponent>,
			UpdateComponent<ScriptComponent>, UpdateComponent<Sprite2DComponent>, UpdateComponent<Text2DComponent>, UpdateComponent<Transform2DComponent>,

			AddEntity, RemoveEntity,

			SelectEntity, DeselectEntity, ClearSelection
		>;

		Variant Data;

		template<typename T>
		EditCommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const EditCommand& command, ProjectSessionCommandContext& context);
}
