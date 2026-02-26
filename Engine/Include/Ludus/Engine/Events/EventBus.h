#pragma once

#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Events/EventType.h>

namespace Ludus::Engine::Events
{
	struct Event;
	struct EventHandler;
}

namespace Ludus::Engine::Events
{
	class EventBus
	{
	private:
		std::unordered_map<EventType, std::vector<EventHandler*>> m_EventHandlers;
		std::unordered_map<EventType, std::vector<const Event*>> m_EventQueue;

	public:
		EventBus() = default;
		~EventBus() = default;

		void Subscribe(EventType type, EventHandler& handler);
		void Unsubscribe(EventType type, EventHandler& handler);

		void PublishEvent(const Event& event);

		void EnqueueEvent(const Event& event);
		void ProcessQueued();

		int GetHandlerCount(EventType type) const;
		int GetEventCount(EventType type) const;
	};
}
