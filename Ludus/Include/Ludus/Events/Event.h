#pragma once

#include <Ludus/Events/EventType.h>

namespace Ludus::Events
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
