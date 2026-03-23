#pragma once

#include <filesystem>
#include <string>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Dialogs/DialogOutcome.h>

namespace Ludus::Editor::Dialogs
{
	struct CreateProjectDialog
	{
	private:
		bool IsOpen = true;
		bool JustOpened = true;
		std::string Error;

		std::string Name;
		std::string DestinationPreview;

		using Outcome = DialogOutcome<std::string>;

	public:

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}
