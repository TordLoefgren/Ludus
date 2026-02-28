#include "pch.h"

#include <type_traits>

#include <Ludus/Editor/Dialogs/CreateProjectDialog.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
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
			Ludus::UI::Context::PopupContext::OpenPopup(popupLabel.c_str(), Ludus::UI::Flags::Popup::None);
			JustOpened = false;
		}

		if (Ludus::UI::Scope::PopupModalScope dialogScope(popupLabel.c_str(), &IsOpen, Ludus::UI::Flags::Window::AlwaysAutoResize); dialogScope)
		{
			Ludus::UI::Widgets::TextUnformatted("Please write a project name:");
			Ludus::UI::Widgets::InputText("##NewProjectName", Name);

			if (!Error.empty())
			{
				Ludus::UI::Widgets::TextUnformatted(Error.c_str());
			}

			if (Ludus::UI::Widgets::Button("Create"))
			{
				if (!Name.empty())
				{
					IsOpen = false;
					return Outcome::Confirm(Name);
				}
				Error = "Name must not be empty.";
			}

			Ludus::UI::Context::LayoutContext::SameLine(0.0f, 6.0f);

			if (Ludus::UI::Widgets::Button("Cancel"))
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
		std::visit([&](auto&& o)
		{
			using O = std::decay_t<decltype(o)>;

			if constexpr (std::is_same_v<O, typename Outcome::None>)
			{
				// Do nothing.
			}
			else if constexpr (std::is_same_v<O, typename Outcome::Cancelled>)
			{
				// Do nothing.
			}
			else if constexpr (std::is_same_v<O, typename Outcome::Confirmed>)
			{
				out.RequestCommands.emplace_back(
					Ludus::Editor::Commands::RequestCommand::CreateProject { o.Payload }
				);
			}
		}, outcome.Data);
	}

	bool CreateProjectDialog::ShouldClose(const Outcome&) const
	{
		return !IsOpen;
	}
}
