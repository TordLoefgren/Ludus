#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/Eventhandler.h>
#include <Ludus/Engine/Events/EventType.h>

namespace Ludus::Engine::Events
{
	class EventBus
	{
	private:
		std::unordered_map<EventType, std::vector<Eventhandler*>> m_EventHandlers;
		std::unordered_map<EventType, std::vector<const Event*>> m_EventQueue;

	public:
		EventBus() = default;
		~EventBus() = default;

		void Subscribe(EventType type, Eventhandler& handler);
		void Unsubscribe(EventType type, Eventhandler& handler);

		void PublishEvent(const Event& event);

		void EnqueueEvent(const Event& event);
		void ProcessQueued();

		int GetHandlerCount(EventType type) const;
		int GetEventCount(EventType type) const;
	};
}
