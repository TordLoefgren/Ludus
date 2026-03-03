#pragma once

#include <variant>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext;

	struct UICommand
	{
		struct OpenAddScriptDialog { Ludus::Engine::Core::EntityHandle Entity; Ludus::Engine::Core::SceneHandle Scene; };
		struct OpenCreateProjectDialog { };

		using Variant = std::variant<OpenAddScriptDialog, OpenCreateProjectDialog>;

		Variant Data;

		template<typename T>
		UICommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const UICommand& command, CommandContext& context);
}
