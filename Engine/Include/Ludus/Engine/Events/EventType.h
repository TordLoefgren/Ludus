#pragma once

#include <format>
#include <string_view>

namespace Ludus::Engine::Events
{
	enum class EventType
	{
		FramebufferSizeEvent,
		KeyEvent,
		MouseButtonEvent,
		MouseEnterEvent,
		MouseMoveEvent,
		MouseScrollEvent,
		TextInputEvent,
		WindowCloseEvent,
		WindowContentScaleEvent,
		WindowFocusEvent,
		WindowIconifyEvent,
		WindowMaximizeEvent,
		WindowPositionEvent,
		WindowRefreshEvent,
		WindowSizeEvent,
	};

	constexpr std::string_view ToString(EventType type)
	{
		switch (type)
		{
			case EventType::FramebufferSizeEvent:		return "FramebufferSizeEvent";
			case EventType::KeyEvent:					return "KeyEvent";
			case EventType::MouseButtonEvent:			return "MouseButtonEvent";
			case EventType::MouseEnterEvent:			return "MouseEnterEvent";
			case EventType::MouseMoveEvent:				return "MouseMoveEvent";
			case EventType::MouseScrollEvent:			return "MouseScrollEvent";
			case EventType::TextInputEvent:				return "TextInputEvent";
			case EventType::WindowCloseEvent:			return "WindowCloseEvent";
			case EventType::WindowContentScaleEvent:	return "WindowContentScaleEvent";
			case EventType::WindowFocusEvent:			return "WindowFocusEvent";
			case EventType::WindowIconifyEvent:			return "WindowIconifyEvent";
			case EventType::WindowMaximizeEvent:		return "WindowMaximizeEvent";
			case EventType::WindowPositionEvent:		return "WindowPositionEvent";
			case EventType::WindowRefreshEvent:			return "WindowRefreshEvent";
			case EventType::WindowSizeEvent:			return "WindowSizeEvent";
			default:									return "Unknown";
		}
	}
}

namespace std
{
	template <>
	struct formatter<Ludus::Engine::Events::EventType> : formatter<string_view>
	{
		auto format(const Ludus::Engine::Events::EventType& type, format_context& context) const
		{
			return formatter<string_view>::format(Ludus::Engine::Events::ToString(type), context);
		}
	};
}
