#include "pch.h"

#include <string>
#include <type_traits>
#include <vector>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Dialogs/AddScriptDialog.h>
#include <Ludus/Editor/Dialogs/DialogHelpers.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/DisabledScope.h>
#include <Ludus/UI/Scope/ModalScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/TabScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Selection.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Dialogs
{
	AddScriptDialog::AddScriptDialog(
		Ludus::Engine::Core::SceneId sceneId,
		Ludus::Engine::Core::EntityId entityId,
		std::vector<std::string> scriptNames,
		std::vector<Ludus::Engine::Runtime::ScriptReference> scriptReferences
	) :
		m_SceneId(sceneId),
		m_EntityId(entityId),
		m_ScriptNames(std::move(scriptNames)),
		m_ScriptReferences(std::move(scriptReferences))
	{}

	AddScriptDialog::Outcome AddScriptDialog::Draw()
	{
		const auto popupLabel = Ludus::UI::CreateLabel("Add Script", "Add Script");

		if (m_JustOpened)
		{
			if (m_ScriptNames.empty())
			{
				m_SelectName = "None";
				m_ScriptNames.push_back(m_SelectName);
			}
			else
			{
				m_SelectName = m_ScriptNames[0];
			}

			Ludus::Editor::Dialogs::CenterNextDialogOnMousePosition();
			Ludus::UI::Context::PopupContext::OpenPopup(popupLabel.c_str(), Ludus::UI::Flags::Popup::None);
			m_JustOpened = false;
		}

		if (Ludus::UI::Scope::PopupModalScope dialogScope(popupLabel.c_str(), &m_IsOpen, Ludus::UI::Flags::Window::AlwaysAutoResize); dialogScope)
		{
			if (Ludus::UI::Scope::TabBarScope tabBarScope("##TabBar"); tabBarScope)
			{
				if (Ludus::UI::Scope::TabItemScope createTabItemScope("Create##Create_TabItem", nullptr); createTabItemScope)
				{
					m_ActiveTab = AddScriptTab::Create;

					Ludus::UI::Widgets::TextUnformatted("Create");
					Ludus::UI::Widgets::InputText("##CreateName", m_CreateName);

					if (!m_Error.empty())
					{
						Ludus::UI::Widgets::TextUnformattedColor(m_Error.c_str(), Ludus::UI::Context::ThemeContext::Error());
					}
				}

				if (Ludus::UI::Scope::TabItemScope selectTabItemScope("Select##Select_TabItem", nullptr); selectTabItemScope)
				{
					m_ActiveTab = AddScriptTab::Select;
					m_Error.clear();

					auto currentIndex = -1;
					auto names = Ludus::UI::Widgets::GetCStringItems(m_ScriptNames, [](const std::string& item)
					{
						return item.c_str();
					});

					if (currentIndex < 0)
					{
						for (auto i = 0; i < static_cast<int>(names.size()); i++)
						{
							const auto& name = names[static_cast<size_t>(i)];
							if (std::string(name) == m_SelectName)
							{
								currentIndex = i;
								break;
							}
						}
					}

					// Default to first item.
					if (currentIndex < 0)
					{
						currentIndex = 0;
					}

					Ludus::UI::Widgets::TextUnformatted("Select");
					Ludus::UI::Scope::DisabledScope disabled(m_ActiveTab == AddScriptTab::Select && m_SelectName == "None");
					if (Ludus::UI::Widgets::Combo("##Select_Combo", &currentIndex, names))
					{
						m_SelectName = m_ScriptNames[currentIndex];
					}
				}
			}

			Ludus::UI::Context::LayoutContext::Separator();

			{
				Ludus::UI::Scope::DisabledScope disabled(
					m_ActiveTab == AddScriptTab::Create && m_CreateName.empty() ||
					m_ActiveTab == AddScriptTab::Select && m_SelectName == "None"
				);

				if (Ludus::UI::Widgets::Button("Create", Ludus::Editor::Core::Constants::Shared::ModalActionButtonSize))
				{
					if (m_ActiveTab == AddScriptTab::Create)
					{
						m_Error = Ludus::Editor::Persistence::ProjectPaths::ValidateFileName(m_CreateName);
						if (m_Error.empty())
						{
							for (const auto& scriptReference : m_ScriptReferences)
							{
								if (scriptReference.Name == m_CreateName)
								{
									m_Error = "Script already exists.";
									break;
								}
							}
						}

						if (!m_Error.empty())
						{
							return Outcome::NoneState();
						}
					}

					m_IsOpen = false;
					return Outcome::Confirm(m_ActiveTab == AddScriptTab::Create ? m_CreateName : m_SelectName);
				}
			}

			Ludus::UI::Context::LayoutContext::SameLine(0.0f, Ludus::Editor::Core::Constants::Shared::StandardInlineSpacing);

			if (Ludus::UI::Widgets::Button("Cancel", Ludus::Editor::Core::Constants::Shared::ModalActionButtonSize))
			{
				m_CreateName.clear();
				m_SelectName.clear();
				m_ScriptNames.clear();
				m_IsOpen = false;

				return Outcome::Cancel();
			}

			return Outcome::NoneState();
		}

		return Outcome::NoneState();
	}

	void AddScriptDialog::Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out)
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
				if (m_ActiveTab == AddScriptTab::Create)
				{
					out.RequestCommands.emplace_back(
						Ludus::Editor::Commands::RequestCommand::CreateScript { m_SceneId, m_EntityId, value.Payload }
					);
					return;
				}

				for (const auto& scriptReference : m_ScriptReferences)
				{
					if (scriptReference.Name != value.Payload)
					{
						continue;
					}

					out.EditCommands.emplace_back(
						Ludus::Editor::Commands::EditCommand::AddComponent<Ludus::Engine::Components::ScriptComponent> {
						.SceneId = m_SceneId, .EntityReference = m_EntityId, .Init = Ludus::Engine::Components::ScriptComponent { scriptReference.Id }
					});
					return;
				}
			}
		}, outcome.Data);
	}

	bool AddScriptDialog::ShouldClose(const Outcome&) const
	{
		return !m_IsOpen;
	}
}
