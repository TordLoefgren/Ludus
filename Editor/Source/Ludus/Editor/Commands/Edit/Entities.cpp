#include "pch.h"

#include <type_traits>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/Edit/Entities.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands::Edit::Entities
{
	void AddEntity(const EditCommand::AddEntity& command, CommandContext& context)
	{
		auto* scene = context.SystemContext.SceneRegistry.TryGetScene(command.Scene);
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
				context.EditorContext.State.Commands.BindEntityReference(value.Temp, handle);
			}
		}, command.Entity.Value);
	}

	void RemoveEntity(const EditCommand::RemoveEntity& command, CommandContext& context)
	{
		auto* scene = context.SystemContext.SceneRegistry.TryGetScene(command.Scene);
		if (!scene)
		{
			return;
		}

		const auto handle = context.EditorContext.State.Commands.ResolveEntity(command.Entity);
		scene->EntityComponentSystem.DestroyEntity(handle);
	}
}
