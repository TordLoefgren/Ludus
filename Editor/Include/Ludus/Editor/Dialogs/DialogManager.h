#pragma once

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Dialogs/AddScriptDialog.h>
#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>

namespace Ludus::Editor::Dialogs
{
	using ActiveDialog = std::variant<std::monostate, AddScriptDialog, CreateProjectDialog>;

	struct DialogManager
	{
		ActiveDialog Active;

		template<typename TDialog>
		void Open() { Active = TDialog { }; }

		template<typename TDialog>
		void Open(TDialog dialog) { Active = std::move(dialog); }

		static void VisitDialog(std::monostate&, Ludus::Editor::Commands::CommandSet& out, bool& hasOut, bool& close)
		{
			// Do nothing.
		}

		template<typename T>
		static void VisitDialog(T& dialog, Ludus::Editor::Commands::CommandSet& out, bool& hasOut, bool& close)
		{
			auto outcome = dialog.Draw();
			dialog.Resolve(outcome, out);

			hasOut = !out.Empty();
			close = dialog.ShouldClose(outcome);
		}

		std::optional<Ludus::Editor::Commands::CommandSet> Update()
		{
			if (std::holds_alternative<std::monostate>(Active))
			{
				return std::nullopt;
			}

			Ludus::Editor::Commands::CommandSet out;
			bool hasOut = false;
			bool close = false;

			std::visit([&](auto& dialog)
			{
				VisitDialog(dialog, out, hasOut, close);
			}, Active);

			if (close)
			{
				Active = std::monostate { };
			}

			if (hasOut)
			{
				return out;
			}

			return std::nullopt;
		}
	};
}

