#pragma once

#include <utility>
#include <variant>

namespace Ludus::Editor::Commands
{
	struct CommandContext;

	struct UICommand
	{
		struct OpenCreateProjectDialog { };

		using Variant = std::variant<OpenCreateProjectDialog>;

		Variant Data;

		template<typename T>
		UICommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const UICommand& command, CommandContext& context);
}

