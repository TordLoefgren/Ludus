#include "pch.h"

#include <type_traits>

#include <Ludus/Editor/Commands/Edit/Entities.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Edit::Entities
{
	void AddEntity(const EditCommand::AddEntity& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneHandle);
		if (!scene)
		{
			return;
		}

		const auto handle = scene->EntityComponentSystem.AddEntity();

		std::visit([&](auto&& value)
		{
			using Alt = std::decay_t<decltype(value)>;
			if constexpr (std::is_same_v<Alt, Ludus::Engine::Core::EntityHandle>)
			{
				LUDUS_ASSERT(false, "AddEntity must use a temporary entity reference.");
			}
			else
			{
				context.Shell.State.Commands.BindEntityReference(value.Temp, handle);
			}
		}, command.EntityReference.Value);

		context.ProjectSession.MarkSceneDirty();
	}

	void RemoveEntity(const EditCommand::RemoveEntity& command, ProjectSessionCommandContext& context)
	{
		auto* scene = context.ProjectSession.GetSceneRegistry().TryGetScene(command.SceneHandle);
		if (!scene)
		{
			return;
		}

		const auto handle = context.Shell.State.Commands.ResolveEntity(command.EntityReference);
		scene->EntityComponentSystem.DestroyEntity(handle);

		context.ProjectSession.MarkSceneDirty();
	}
}
