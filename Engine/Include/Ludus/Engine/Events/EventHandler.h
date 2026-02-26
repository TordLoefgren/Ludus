#pragma once

namespace Ludus::Engine::Events
{
	struct Event;
}

namespace Ludus::Engine::Events
{
	struct EventHandler
	{
		virtual ~EventHandler() = default;
		virtual bool ProcessEvent(const Event& event) = 0;
	};
}
