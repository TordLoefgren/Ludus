#include "pch.h"

#include <string>

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Elements/ActionTreeNode.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/StyleScope.h>

namespace Ludus::UI::Elements
{
	ActionTreeNode::ActionTreeNode(const ActionTreeNodeOptions& options, ActionTreeNodeResult* result)
	{
		if (result)
		{
			*result = { };
		}

		const auto treeNodeId = Ludus::UI::CreateLabel(options.Label, options.Id);
		const auto buttonId = Ludus::UI::CreateLabel(options.MenuButtonIcon, std::string(options.Id).append("_MenuButton"));
		m_MenuPopupId = Ludus::UI::CreateLabel(options.MenuButtonIcon, options.MenuId);

		m_Open = ImGui::TreeNodeEx(
			treeNodeId.c_str(),
			static_cast<int>(options.Flags | Ludus::UI::Flags::TreeNode::AllowOverlap)
		);

		if (result)
		{
			result->IsOpen = m_Open;
		}

		const auto contentCursorPos = ImGui::GetCursorPos();
		const auto headerRightX = ImGui::GetItemRectMax().x;
		const auto headerTopY = ImGui::GetItemRectMin().y;

		{
			Ludus::UI::Scope::StyleVarScope styleVar(
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::FrameBorderSize, 0.0f)
			);

			Ludus::UI::Scope::StyleColorScope colorScope({
				{ Ludus::UI::Scope::Color::Button, Ludus::Engine::Graphics::Colors::Transparent },
				{ Ludus::UI::Scope::Color::ButtonHovered, Ludus::UI::Context::ThemeContext::ControlHover() },
				{ Ludus::UI::Scope::Color::ButtonActive, Ludus::UI::Context::ThemeContext::ControlActive() }
				});

			const auto buttonLabelSize = ImGui::CalcTextSize(buttonId.c_str(), nullptr, true);
			const auto buttonWidth = buttonLabelSize.x + ImGui::GetStyle().FramePadding.x * 2.0f;

			ImGui::SetNextItemAllowOverlap();
			ImGui::SetCursorScreenPos({ headerRightX - buttonWidth, headerTopY });

			if (ImGui::Button(buttonId.c_str()))
			{
				Ludus::UI::Context::PopupContext::OpenPopup(m_MenuPopupId.c_str());

				if (result)
				{
					result->IsActionTriggered = true;
				}
			}

			if (result)
			{
				result->IsMenuOpen = ImGui::IsPopupOpen(m_MenuPopupId.c_str());
			}
		}

		ImGui::SetCursorPos(contentCursorPos);
	}

	Ludus::UI::Scope::PopupScope ActionTreeNode::Menu() const
	{
		return Ludus::UI::Scope::PopupScope(m_MenuPopupId.c_str());
	}

	ActionTreeNode::~ActionTreeNode()
	{
		if (m_Open)
		{
			ImGui::TreePop();
		}
	}
}
