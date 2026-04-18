#include "pch.h"

#include <type_traits>
#include <utility>

#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Dialogs/DialogHelpers.h>
#include <Ludus/Editor/Dialogs/UnsavedChangesDialog.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ModalScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Dialogs
{
	UnsavedChangesDialog::UnsavedChangesDialog(
		Ludus::Editor::Commands::Requests::DeferredAction deferredAction
	) :
		m_DeferredAction(std::move(deferredAction))
	{}

	UnsavedChangesDialog::Outcome UnsavedChangesDialog::Draw()
	{
		const auto popupLabel = Ludus::UI::CreateLabel("Unsaved Changes", "Unsaved Changes");

		if (m_JustOpened)
		{
			Ludus::Editor::Dialogs::CenterNextDialogOnMainViewport();
			Ludus::UI::Context::PopupContext::OpenPopup(popupLabel.c_str(), Ludus::UI::Flags::Popup::None);
			m_JustOpened = false;
		}

		if (Ludus::UI::Scope::PopupModalScope dialogScope(popupLabel.c_str(), &m_IsOpen, Ludus::UI::Flags::Window::AlwaysAutoResize); dialogScope)
		{
			Ludus::UI::Widgets::TextUnformatted("Save changes?");

			if (Ludus::UI::Widgets::Button("Save", Ludus::Editor::Core::Constants::Shared::ModalActionButtonSize))
			{
				m_IsOpen = false;
				return Outcome::Confirm(UnsavedChangesResult::Save);
			}

			Ludus::UI::Context::LayoutContext::SameLine(0.0f, Ludus::Editor::Core::Constants::Shared::StandardInlineSpacing);

			if (Ludus::UI::Widgets::Button("Don't save", Ludus::Editor::Core::Constants::Shared::ModalActionButtonSize))
			{
				m_IsOpen = false;
				return Outcome::Confirm(UnsavedChangesResult::DontSave);
			}

			Ludus::UI::Context::LayoutContext::SameLine(0.0f, Ludus::Editor::Core::Constants::Shared::StandardInlineSpacing);

			if (Ludus::UI::Widgets::Button("Cancel", Ludus::Editor::Core::Constants::Shared::ModalActionButtonSize))
			{
				m_IsOpen = false;
				return Outcome::Confirm(UnsavedChangesResult::Cancel);
			}

			return Outcome::NoneState();
		}

		return Outcome::NoneState();
	}

	void UnsavedChangesDialog::Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out)
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
					Ludus::Editor::Commands::RequestCommand::ResolveUnsavedChanges { .DeferredAction = m_DeferredAction, .Result = value.Payload }
				);
			}
		}, outcome.Data);
	}

	bool UnsavedChangesDialog::ShouldClose(const Outcome&) const
	{
		return !m_IsOpen;
	}
}
