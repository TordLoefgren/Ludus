#include "pch.h"

#include <Ludus/Editor/Commands/UI/Dialogs.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Engine/Core/Variants.h>

namespace Ludus::Editor::Commands
{
	namespace
	{
		struct UICommandVisitor
		{
			CommandContext& Context;

			void operator()(const UICommand::OpenAddScriptDialog& command) const { UI::Dialogs::OpenAddScriptDialog(command, Context); }
			void operator()(const UICommand::OpenCreateProjectDialog& command) const { UI::Dialogs::OpenCreateProjectDialog(command, Context); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};
	}

	void Execute(const UICommand& command, CommandContext& context)
	{
		std::visit(UICommandVisitor { context }, command.Data);
	}
}
