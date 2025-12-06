#pragma once

#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventType.h>

namespace Ludus::Engine::Events::WindowEvents
{
	struct FramebufferSizeEvent final : Event
	{
		int Width;
		int Height;

		FramebufferSizeEvent(int width, int height) : Width(width), Height(height), Event(EventType::FramebufferSizeEvent) { }
	};

	struct WindowCloseEvent final : Event
	{
		WindowCloseEvent() : Event(EventType::WindowCloseEvent) { }
	};

	struct WindowContentScaleEvent final : Event
	{
		float XScale;
		float YScale;

		WindowContentScaleEvent(float xScale, float yScale) : XScale(xScale), YScale(yScale), Event(EventType::WindowContentScaleEvent) { }
	};

	struct WindowFocusEvent final : Event
	{
		bool Focused;

		WindowFocusEvent(bool focused) : Focused(focused), Event(EventType::WindowFocusEvent) { }
	};

	struct WindowIconifyEvent final : Event
	{
		bool Iconified;

		WindowIconifyEvent(bool iconified) : Iconified(iconified), Event(EventType::WindowIconifyEvent) { }
	};

	struct WindowMaximizeEvent final : Event
	{
		bool Maximized;

		WindowMaximizeEvent(bool maximized) : Maximized(maximized), Event(EventType::WindowMaximizeEvent) { }
	};

	struct WindowPositionEvent final : Event
	{
		int X;
		int Y;

		WindowPositionEvent(int x, int y) : X(x), Y(y), Event(EventType::WindowPositionEvent) { }
	};

	struct WindowRefreshEvent final : Event
	{
		WindowRefreshEvent() : Event(EventType::WindowRefreshEvent) { }
	};

	struct WindowSizeEvent final : Event
	{
		int Width;
		int Height;

		WindowSizeEvent(int width, int height) : Width(width), Height(height), Event(EventType::WindowSizeEvent) { }
	};
}
