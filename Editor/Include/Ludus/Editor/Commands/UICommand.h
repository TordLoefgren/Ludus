#pragma once

#include <utility>
#include <variant>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Commands
{
	struct StartupCommandContext;
	struct ProjectSessionCommandContext;

	struct UICommand
	{
		struct OpenAddScriptDialog { Ludus::Engine::Core::EntityHandle EntityHandle; Ludus::Engine::Core::SceneHandle SceneHandle; };
		struct OpenCreateProjectDialog { };

		using Variant = std::variant<OpenAddScriptDialog, OpenCreateProjectDialog>;

		Variant Data;

		template<typename T>
		UICommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const UICommand& command, StartupCommandContext& context);
	void Execute(const UICommand& command, ProjectSessionCommandContext& context);
}
