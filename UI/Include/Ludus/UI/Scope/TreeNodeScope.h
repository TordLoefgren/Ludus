#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::TreeNode DefaultTreeNodeFlags =
		Ludus::UI::Flags::TreeNode::DefaultOpen | Ludus::UI::Flags::TreeNode::Framed;

	class TreeNodeScope
	{
	private:
		bool m_Open = false;

	public:
		explicit TreeNodeScope(const char* label, Ludus::UI::Flags::TreeNode flags = DefaultTreeNodeFlags);

		~TreeNodeScope();

		explicit operator bool() const { return m_Open; }
	};
}
