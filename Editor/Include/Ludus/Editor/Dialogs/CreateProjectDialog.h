#pragma once

#include <string>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Dialogs/DialogOutcome.h>

namespace Ludus::Editor::Dialogs
{
	struct CreateProjectDialog
	{
		bool IsOpen = true;
		bool JustOpened = true;
		std::string Name;
		std::string Error;

		using Outcome = DialogOutcome<std::string>;

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}

