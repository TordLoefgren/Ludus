#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::Popup DefaultPopupContextItemFlags = Ludus::UI::Flags::Popup::MouseButtonRight;
	constexpr Ludus::UI::Flags::Window DefaultPopupFlags = Ludus::UI::Flags::Window::None;

	class PopupScope
	{
	private:
		bool m_Open = false;

	public:
		explicit PopupScope(const char* label = nullptr, Ludus::UI::Flags::Window flags = DefaultPopupFlags);

		~PopupScope();

		explicit operator bool() const { return m_Open; }
	};

	class PopupContextItemScope
	{
	private:
		bool m_Open = false;

	public:
		explicit PopupContextItemScope(const char* label = nullptr, Ludus::UI::Flags::Popup flags = DefaultPopupContextItemFlags);

		~PopupContextItemScope();

		explicit operator bool() const { return m_Open; }
	};

	constexpr Ludus::UI::Flags::Popup DefaultPopupContextWindowFlags = Ludus::UI::Flags::Popup::None;

	class PopupContextWindowScope
	{
	private:
		bool m_Active = false;

	public:
		explicit PopupContextWindowScope(const char* id, Ludus::UI::Flags::Popup flags = DefaultPopupContextWindowFlags);

		~PopupContextWindowScope();

		explicit operator bool() const { return m_Active; }
	};
}
