#pragma once

#include <utility>
#include <variant>

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Commands
{
	struct StartupCommandContext;
	struct ProjectSessionCommandContext;

	struct UICommand
	{
		struct OpenAddScriptDialog { Ludus::Engine::Core::SceneId SceneId; Ludus::Engine::Core::EntityId EntityId; };
		struct OpenCreateProjectDialog { };
		struct OpenRenameSceneDialog { Ludus::Engine::Core::SceneId SceneId; };

		using Variant = std::variant<OpenAddScriptDialog, OpenCreateProjectDialog, OpenRenameSceneDialog>;

		Variant Data;

		template<typename T>
		UICommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const UICommand& command, StartupCommandContext& context);
	void Execute(const UICommand& command, ProjectSessionCommandContext& context);
}
