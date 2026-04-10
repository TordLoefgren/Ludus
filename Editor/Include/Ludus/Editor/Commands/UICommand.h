#pragma once

#include <utility>
#include <variant>

#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
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
		struct OpenUnsavedChangesDialog { Ludus::Editor::Commands::Requests::DeferredAction DeferredAction; };

		using Variant = std::variant<OpenAddScriptDialog, OpenCreateProjectDialog, OpenRenameSceneDialog, OpenUnsavedChangesDialog>;

		Variant Data;

		template<typename T>
		UICommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const UICommand& command, StartupCommandContext& context);
	void Execute(const UICommand& command, ProjectSessionCommandContext& context);
}
