#pragma once

#include <Ludus/Engine/Events/EventType.h>

namespace Ludus::Engine::Events
{
	struct Event
	{
		const EventType Type;

	protected:
		explicit Event(const EventType type) : Type(type) { }

	public:
		virtual ~Event() = default;
	};
}
