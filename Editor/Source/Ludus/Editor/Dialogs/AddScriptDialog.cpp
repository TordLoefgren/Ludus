#include "pch.h"

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Dialogs/AddScriptDialog.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/DisabledScope.h>
#include <Ludus/UI/Scope/ModalScope.h>
#include <Ludus/UI/Scope/TabScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Selection.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Dialogs
{
	AddScriptDialog::AddScriptDialog(
		Ludus::Engine::Core::EntityHandle entityHandle,
		Ludus::Engine::Core::SceneHandle sceneHandle,
		std::vector<std::string> scriptNames,
		std::unordered_map<std::string, Ludus::Engine::Components::ScriptHandle> scriptHandlesByName
	) :
		EntityHandle(entityHandle),
		SceneHandle(sceneHandle),
		ScriptNames(std::move(scriptNames)),
		ScriptHandlesByName(std::move(scriptHandlesByName))
	{ }

	AddScriptDialog::Outcome AddScriptDialog::Draw()
	{
		const auto popupLabel = Ludus::UI::CreateLabel("Add Script", "Add Script");

		if (JustOpened)
		{
			if (ScriptNames.empty())
			{
				SelectName = "None";
				ScriptNames.push_back(SelectName);
			}
			else
			{
				SelectName = ScriptNames[0];
			}

			Ludus::UI::Context::PopupContext::OpenPopup(popupLabel.c_str(), Ludus::UI::Flags::Popup::None);
			JustOpened = false;
		}

		Ludus::UI::Context::WindowContext::SetNextWindowSize(Ludus::Editor::Core::Constants::AddScriptDialogSize);

		if (Ludus::UI::Scope::PopupModalScope dialogScope(popupLabel.c_str(), &IsOpen); dialogScope)
		{
			if (Ludus::UI::Scope::TabBarScope tabBarScope("##TabBar"); tabBarScope)
			{
				if (Ludus::UI::Scope::TabItemScope createTabItemScope("Create##Create_TabItem", nullptr); createTabItemScope)
				{
					ActiveTab = AddScriptTab::Create;

					Ludus::UI::Widgets::TextUnformatted("Create");
					Ludus::UI::Widgets::InputText("##CreateName", CreateName);

					if (!Error.empty())
					{
						Ludus::UI::Widgets::TextUnformatted(Error.c_str());
					}
				}

				if (Ludus::UI::Scope::TabItemScope selectTabItemScope("Select##Select_TabItem", nullptr); selectTabItemScope)
				{
					ActiveTab = AddScriptTab::Select;
					Error.clear();

					auto currentIndex = -1;
					auto names = Ludus::UI::Widgets::GetCStringItems(ScriptNames, [](const std::string& item)
					{
						return item.c_str();
					});

					if (currentIndex < 0)
					{
						for (auto i = 0; i < static_cast<int>(names.size()); i++)
						{
							const auto& name = names[static_cast<size_t>(i)];
							if (std::string(name) == SelectName)
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
					Ludus::UI::Scope::DisabledScope disabled(ActiveTab == AddScriptTab::Select && SelectName == "None");
					if (Ludus::UI::Widgets::Combo("##Select_Combo", &currentIndex, names))
					{
						SelectName = ScriptNames[currentIndex];
					}
				}
			}

			Ludus::UI::Context::LayoutContext::Separator();

			{
				Ludus::UI::Scope::DisabledScope disabled(
					ActiveTab == AddScriptTab::Create && CreateName.empty() ||
					ActiveTab == AddScriptTab::Select && SelectName == "None"
				);

				if (Ludus::UI::Widgets::Button("Create", Ludus::Editor::Core::Constants::ModalActionButtonSize))
				{
					if (ActiveTab == AddScriptTab::Create)
					{
						Error = Ludus::Editor::Persistence::Paths::ValidateScriptName(CreateName);
						if (Error.empty() && ScriptHandlesByName.find(CreateName) != ScriptHandlesByName.end())
						{
							Error = "Script already exists.";
						}

						if (!Error.empty())
						{
							return Outcome::NoneState();
						}
					}

					IsOpen = false;
					return Outcome::Confirm(ActiveTab == AddScriptTab::Create ? CreateName : SelectName);
				}
			}

			Ludus::UI::Context::LayoutContext::SameLine(0.0f, Ludus::Editor::Core::Constants::StandardInlineSpacing);

			if (Ludus::UI::Widgets::Button("Cancel", Ludus::Editor::Core::Constants::ModalActionButtonSize))
			{
				CreateName.clear();
				SelectName.clear();
				ScriptNames.clear();
				IsOpen = false;

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
				if (ActiveTab == AddScriptTab::Create)
				{
					out.RequestCommands.emplace_back(
						Ludus::Editor::Commands::RequestCommand::CreateScript { EntityHandle, SceneHandle, value.Payload }
					);
					return;
				}

				const auto handle = ScriptHandlesByName.find(value.Payload);
				if (handle == ScriptHandlesByName.end())
				{
					return;
				}

				out.EditCommands.emplace_back(
					Ludus::Editor::Commands::EditCommand::AddComponent<Ludus::Engine::Components::ScriptComponent> {
					.EntityReference = EntityHandle, .SceneHandle = SceneHandle, .Init = Ludus::Engine::Components::ScriptComponent { value.Payload, handle->second }
				});
			}
		}, outcome.Data);
	}

	bool AddScriptDialog::ShouldClose(const Outcome&) const
	{
		return !IsOpen;
	}
}
