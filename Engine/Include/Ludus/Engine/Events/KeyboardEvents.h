#pragma once

#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventType.h>

namespace Ludus::Engine::Events::KeyboardEvents
{
	struct TextInputEvent final : Event
	{
		unsigned int Codepoint;

		TextInputEvent(unsigned int codepoint)
			: Codepoint(codepoint), Event(EventType::TextInputEvent)
		{ }
	};

	struct KeyEvent final : Event
	{
		int Key;
		int Scancode;
		int Action;
		int Mods;

		KeyEvent(int key, int scancode, int action, int mods)
			: Key(key), Scancode(scancode), Action(action), Mods(mods), Event(EventType::KeyEvent)
		{ }
	};
}
