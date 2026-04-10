#pragma once

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Dialogs/DialogOutcome.h>
#include <Ludus/Editor/Dialogs/UnsavedChangesResult.h>

namespace Ludus::Editor::Dialogs
{
	struct UnsavedChangesDialog
	{
	private:
		bool m_IsOpen = true;
		bool m_JustOpened = true;
		Ludus::Editor::Commands::Requests::DeferredAction m_DeferredAction;

		using Outcome = DialogOutcome<UnsavedChangesResult>;

	public:
		UnsavedChangesDialog(Ludus::Editor::Commands::Requests::DeferredAction deferredAction);

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}
