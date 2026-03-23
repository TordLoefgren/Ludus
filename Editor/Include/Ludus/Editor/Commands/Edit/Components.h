#pragma once

#include <type_traits>
#include <utility>
#include <variant>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Edit::Components
{

#pragma region Template specializations

	template<typename TComponent>
	struct RegistrySelector;

	template <>
	struct RegistrySelector<Ludus::Engine::Components::Camera2DComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.Cameras; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::Collider2DComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.Colliders; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::DisplayNameComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.DisplayNames; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::RigidBody2DComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.RigidBodies; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::ScriptComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.Scripts; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::Sprite2DComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.Sprites; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::Text2DComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.Texts; }
	};

	template <>
	struct RegistrySelector<Ludus::Engine::Components::Transform2DComponent>
	{
		static auto& Get(Ludus::Engine::Core::EntityComponentSystem& ecs) { return ecs.Transforms; }
	};

	template<typename TComponent>
	auto& RegistryFor(Ludus::Engine::Core::EntityComponentSystem& ecs)
	{
		return RegistrySelector<TComponent>::Get(ecs);
	}

#pragma endregion

	template<typename TComponent>
	void AddComponent(const EditCommand::AddComponent<TComponent>& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneHandle);
		if (!scene)
		{
			return;
		}

		auto& registry = RegistryFor<TComponent>(scene->EntityComponentSystem);
		const auto owner = context.Shell.State.Commands.ResolveEntity(command.EntityReference);

		std::visit([&](auto&& value)
		{
			using Alt = std::decay_t<decltype(value)>;

			if constexpr (std::is_same_v<Alt, Ludus::Editor::Commands::EditCommand::UseDefault>)
			{
				registry.Add(owner);
			}
			else
			{
				auto component = value;
				component.OwnerHandle = owner;
				registry.Add(std::move(component));
			}
		}, command.Init);

		context.ProjectSession.MarkSceneDirty();
	}

	template<typename TComponent>
	void RemoveComponent(const EditCommand::RemoveComponent<TComponent>& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneHandle);
		if (!scene)
		{
			return;
		}

		auto& registry = RegistryFor<TComponent>(scene->EntityComponentSystem);
		const auto owner = context.Shell.State.Commands.ResolveEntity(command.EntityReference);

		registry.RemoveByOwner(owner);

		context.ProjectSession.MarkSceneDirty();
	}

	template<typename TComponent>
	void UpdateComponent(const EditCommand::UpdateComponent<TComponent>& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneHandle);
		if (!scene)
		{
			return;
		}

		auto& registry = RegistryFor<TComponent>(scene->EntityComponentSystem);
		const auto owner = context.Shell.State.Commands.ResolveEntity(command.EntityReference);

		auto after = command.After;
		after.OwnerHandle = owner;

		registry.RemoveByOwner(owner);
		registry.Add(after);

		context.ProjectSession.MarkSceneDirty();
	}
}
