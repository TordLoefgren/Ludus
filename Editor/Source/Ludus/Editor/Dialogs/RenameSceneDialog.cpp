#include "pch.h"

#include <filesystem>
#include <type_traits>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Dialogs/RenameSceneDialog.h>
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
	RenameSceneDialog::RenameSceneDialog(Ludus::Engine::Core::SceneHandle sceneHandle, std::filesystem::path currentPath)
		: SceneHandle(sceneHandle), CurrentPath(currentPath)
	{ }

	RenameSceneDialog::Outcome RenameSceneDialog::Draw()
	{
		const auto popupLabel = Ludus::UI::CreateLabel("Rename Scene", "Rename Scene");

		if (JustOpened)
		{
			Ludus::UI::Context::PopupContext::OpenPopup(popupLabel.c_str(), Ludus::UI::Flags::Popup::None);
			JustOpened = false;
		}

		if (Ludus::UI::Scope::PopupModalScope dialogScope(popupLabel.c_str(), &IsOpen, Ludus::UI::Flags::Window::AlwaysAutoResize); dialogScope)
		{
			Ludus::UI::Widgets::TextUnformatted("Please write a new scene name:");
			if (Ludus::UI::Widgets::InputText("##NewScene", Name))
			{
				NewPath = Ludus::Editor::Persistence::ProjectPaths::SceneFileInDirectory(CurrentPath.parent_path(), Name);
			}

			if (!Error.empty())
			{
				Ludus::UI::Widgets::TextUnformattedColor(Error.c_str(), Ludus::UI::Context::ThemeContext::Error());
			}

			if (Ludus::UI::Widgets::Button("Rename", Ludus::Editor::Core::Constants::ModalActionButtonSize))
			{
				Error = Ludus::Editor::Persistence::ProjectPaths::ValidateFileName(Name);
				if (Error.empty())
				{
					Error = Ludus::Editor::Persistence::ProjectPaths::ValidateAvailablePath(NewPath);
				}

				if (Error.empty())
				{
					IsOpen = false;
					return Outcome::Confirm({ .SceneHandle = SceneHandle, .Path = NewPath });
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

	void RenameSceneDialog::Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out)
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
					Ludus::Editor::Commands::RequestCommand::RenameScene { value.Payload.SceneHandle, value.Payload.Path }
				);
			}
		}, outcome.Data);
	}

	bool RenameSceneDialog::ShouldClose(const Outcome&) const
	{
		return !IsOpen;
	}
}
