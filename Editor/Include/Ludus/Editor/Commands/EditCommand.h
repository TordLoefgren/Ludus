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
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext;

	struct EditCommand
	{
		// Component commands.

		using EntityHandle = Ludus::Engine::Core::EntityHandle;
		using SceneHandle = Ludus::Engine::Core::SceneHandle;
		using Camera2DComponent = Ludus::Engine::Components::Camera2DComponent;
		using Collider2DComponent = Ludus::Engine::Components::Collider2DComponent;
		using DisplayNameComponent = Ludus::Engine::Components::DisplayNameComponent;
		using RigidBody2DComponent = Ludus::Engine::Components::RigidBody2DComponent;
		using Sprite2DComponent = Ludus::Engine::Components::Sprite2DComponent;
		using Text2DComponent = Ludus::Engine::Components::Text2DComponent;
		using Transform2DComponent = Ludus::Engine::Components::Transform2DComponent;

		struct UseDefault { };

		template<typename TComponent>
		struct AddComponent { EntityReference Entity {}; SceneHandle Scene; std::variant<UseDefault, TComponent> Init { UseDefault{} }; };

		template<typename TComponent>
		struct RemoveComponent { EntityReference Entity; SceneHandle Scene; };

		template<typename TComponent>
		struct UpdateComponent { EntityReference Entity; SceneHandle Scene; TComponent Before; TComponent After; };

		// Entity commands.

		struct AddEntity { EntityReference Entity; SceneHandle Scene; };
		struct RemoveEntity { EntityReference Entity; SceneHandle Scene; };

		// Selection commands. 

		struct SelectEntity { EntityReference Entity; };
		struct DeselectEntity { EntityReference Entity; };
		struct ClearSelection { };

		using Variant = std::variant<
			AddComponent<Camera2DComponent>, AddComponent<Collider2DComponent>, AddComponent<DisplayNameComponent>, AddComponent<RigidBody2DComponent>,
			AddComponent<Sprite2DComponent>, AddComponent<Text2DComponent>, AddComponent<Transform2DComponent>,

			RemoveComponent<Camera2DComponent>, RemoveComponent<Collider2DComponent>, RemoveComponent<DisplayNameComponent>, RemoveComponent<RigidBody2DComponent>,
			RemoveComponent<Sprite2DComponent>, RemoveComponent<Text2DComponent>, RemoveComponent<Transform2DComponent>,

			UpdateComponent<Camera2DComponent>, UpdateComponent<Collider2DComponent>, UpdateComponent<DisplayNameComponent>, UpdateComponent<RigidBody2DComponent>,
			UpdateComponent<Sprite2DComponent>, UpdateComponent<Text2DComponent>, UpdateComponent<Transform2DComponent>,

			AddEntity, RemoveEntity,

			SelectEntity, DeselectEntity, ClearSelection
		>;

		Variant Data;

		template<typename T>
		EditCommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const EditCommand& command, CommandContext& context);
}
