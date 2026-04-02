#pragma once

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Scope/PopupScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>

namespace Ludus::UI::Elements
{
	struct ActionTreeNodeOptions
	{
		const char* Id;
		const char* MenuId;
		const char* Label;
		const char* MenuButtonIcon;
		Ludus::UI::Flags::TreeNode Flags = Ludus::UI::Scope::DefaultTreeNodeFlags;
	};

	struct ActionTreeNodeResult
	{
		bool IsOpen = false;
		bool IsMenuOpen = false;
		bool IsActionTriggered = false;
	};

	class ActionTreeNode
	{
	public:
		explicit ActionTreeNode(const ActionTreeNodeOptions& options, ActionTreeNodeResult* result = nullptr);
		~ActionTreeNode();

		Ludus::UI::Scope::PopupScope Menu() const;

		explicit operator bool() const { return m_Open; }

	private:
		bool m_Open = false;
		std::string m_MenuPopupId;
	};
}
