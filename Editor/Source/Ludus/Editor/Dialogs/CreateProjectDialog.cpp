#include "pch.h"

#include <type_traits>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>
#include <Ludus/Editor/Dialogs/DialogHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ModalScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Dialogs
{
	CreateProjectDialog::Outcome CreateProjectDialog::Draw()
	{
		const auto popupLabel = Ludus::UI::CreateLabel("Create Project", "Create Project");

		if (JustOpened)
		{
			Ludus::Editor::Dialogs::CenterNextDialogOnMainViewport();
			Ludus::UI::Context::PopupContext::OpenPopup(popupLabel.c_str(), Ludus::UI::Flags::Popup::None);
			JustOpened = false;
		}

		if (Ludus::UI::Scope::PopupModalScope dialogScope(popupLabel.c_str(), &IsOpen, Ludus::UI::Flags::Window::AlwaysAutoResize); dialogScope)
		{
			const auto projectDirectory = Ludus::Editor::Persistence::ProjectPaths::ProjectRoot(Name);

			Ludus::UI::Widgets::TextUnformatted("Please write a project name:");
			Ludus::UI::Widgets::InputText("##NewProjectName", Name);

			if (!Error.empty())
			{
				Ludus::UI::Widgets::TextUnformattedColor(Error.c_str(), Ludus::UI::Context::ThemeContext::Error());
			}

			if (Ludus::UI::Widgets::Button("Create", Ludus::Editor::Core::Constants::ModalActionButtonSize))
			{
				Error = Ludus::Editor::Persistence::ProjectPaths::ValidateFileName(Name);
				if (Error.empty())
				{
					Error = Ludus::Editor::Persistence::ProjectPaths::ValidateAvailablePath(projectDirectory);
				}

				if (Error.empty())
				{
					IsOpen = false;
					return Outcome::Confirm(Name);
				}
			}

			Ludus::UI::Context::LayoutContext::SameLine(0.0f, Ludus::Editor::Core::Constants::StandardInlineSpacing);

			if (Ludus::UI::Widgets::Button("Cancel", Ludus::Editor::Core::Constants::ModalActionButtonSize))
			{
				Name.clear();
				IsOpen = false;
				return Outcome::Cancel();
			}

			return Outcome::NoneState();
		}

		return Outcome::NoneState();
	}

	void CreateProjectDialog::Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out)
	{
		std::visit([&](auto&& value)
		{
			using Alt = std::decay_t<decltype(value)>;

			if constexpr (std::is_same_v<Alt, typename Outcome::None>)
			{
				// Do nothing.
			}
			else if constexpr (std::is_same_v<Alt, typename Outcome::Cancelled>)
			{
				// Do nothing.
			}
			else if constexpr (std::is_same_v<Alt, typename Outcome::Confirmed>)
			{
				out.RequestCommands.emplace_back(
					Ludus::Editor::Commands::RequestCommand::CreateProject { value.Payload }
				);
			}
		}, outcome.Data);
	}

	bool CreateProjectDialog::ShouldClose(const Outcome&) const
	{
		return !IsOpen;
	}
}
