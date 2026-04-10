#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Commands/UI/Dialogs.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Engine/Core/Variants.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands
{
	namespace
	{
		struct OpenProjectUICommandVisitor
		{
			ProjectSessionCommandContext& Context;

			void operator()(const UICommand::OpenAddScriptDialog& command) const { UI::Dialogs::OpenAddScriptDialog(command, Context); }
			void operator()(const UICommand::OpenCreateProjectDialog&) const { UI::Dialogs::OpenCreateProjectDialog(Context); }
			void operator()(const UICommand::OpenRenameSceneDialog& command) const { UI::Dialogs::OpenRenameSceneDialog(command, Context); }
			void operator()(const UICommand::OpenUnsavedChangesDialog& command) const { UI::Dialogs::OpenUnsavedChangesDialog(command, Context); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};

		struct StartupUICommandVisitor
		{
			StartupCommandContext& Context;

			void operator()(const UICommand::OpenAddScriptDialog&) const { LUDUS_ASSERT(false, "OpenAddScriptDialog is unavailable during startup."); }
			void operator()(const UICommand::OpenCreateProjectDialog& command) const { UI::Dialogs::OpenCreateProjectDialog(Context); }
			void operator()(const UICommand::OpenRenameSceneDialog&) const { LUDUS_ASSERT(false, "OpenRenameSceneDialog is unavailable during startup."); }
			void operator()(const UICommand::OpenUnsavedChangesDialog&) const { LUDUS_ASSERT(false, "OpenUnsavedChangesDialog is unavailable during startup."); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};
	}

	void Execute(const UICommand& command, ProjectSessionCommandContext& context)
	{
		std::visit(OpenProjectUICommandVisitor { context }, command.Data);
	}

	void Execute(const UICommand& command, StartupCommandContext& context)
	{
		std::visit(StartupUICommandVisitor { context }, command.Data);
	}
}
