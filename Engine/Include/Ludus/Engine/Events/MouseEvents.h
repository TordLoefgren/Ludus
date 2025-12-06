#pragma once

#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventType.h>

namespace Ludus::Engine::Events::MouseEvents
{
	struct MouseButtonEvent final : Event
	{
		int Button;
		int Action;
		int Mods;

		MouseButtonEvent(int button, int action, int mods) : Button(button), Action(action), Mods(mods), Event(EventType::MouseButtonEvent) { }
	};

	struct MouseMoveEvent final : Event
	{
		double XPosition;
		double YPosition;

		MouseMoveEvent(double xPosition, double yPosition) : XPosition(xPosition), YPosition(yPosition), Event(EventType::MouseMoveEvent) { }
	};

	struct MouseScrollEvent final : Event
	{
		double XOffset;
		double YOffset;

		MouseScrollEvent(double xOffset, double yOffset) : XOffset(xOffset), YOffset(yOffset), Event(EventType::MouseScrollEvent) { }
	};

	struct MouseEnterEvent final : Event
	{
		bool Entered;

		MouseEnterEvent(bool entered) : Entered(entered), Event(EventType::MouseEnterEvent) { }
	};
}
