#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::TabBar DefaultTabBarFlags = Ludus::UI::Flags::TabBar::None;
	constexpr Ludus::UI::Flags::TabItem DefaultTabItemFlags = Ludus::UI::Flags::TabItem::None;

	class TabBarScope
	{
	private:
		bool m_Open = false;

	public:
		explicit TabBarScope(const char* label, Ludus::UI::Flags::TabBar flags = DefaultTabBarFlags);

		~TabBarScope();

		explicit operator bool() const { return m_Open; }
	};

	class TabItemScope
	{
	private:
		bool m_Open = false;

	public:
		explicit TabItemScope(const char* label, bool* open, Ludus::UI::Flags::TabItem flags = DefaultTabItemFlags);

		~TabItemScope();

		explicit operator bool() const { return m_Open; }
	};
}
