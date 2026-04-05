#include "pch.h"

#include <type_traits>

#include <Ludus/Editor/Commands/Edit/Entities.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Edit::Entities
{
	void AddEntity(const EditCommand::AddEntity& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneId);
		if (!scene)
		{
			return;
		}

		const auto id = scene->EntityComponentSystem.AddEntity();

		std::visit([&](auto&& value)
		{
			using Alt = std::decay_t<decltype(value)>;
			if constexpr (std::is_same_v<Alt, Ludus::Engine::Core::EntityId>)
			{
				LUDUS_ASSERT(false, "AddEntity must use a temporary entity reference.");
			}
			else
			{
				context.Shell.State.Commands.BindEntityReference(value.Temp, id);
			}
		}, command.EntityReference.Value);

		context.ProjectSession.MarkSceneDirty();
	}

	void RemoveEntity(const EditCommand::RemoveEntity& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneId);
		if (!scene)
		{
			return;
		}

		const auto id = context.Shell.State.Commands.ResolveEntity(command.EntityReference);
		scene->EntityComponentSystem.DestroyEntity(id);

		context.ProjectSession.MarkSceneDirty();
	}
}
