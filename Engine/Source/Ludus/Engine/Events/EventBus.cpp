#include "Ludus/Engine/Events/EventBus.h"

#include <format>

#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Events
{
	void EventBus::Subscribe(EventType type, Eventhandler& handler)
	{
		auto& handlers = m_EventHandlers[type];

		if (std::find(handlers.begin(), handlers.end(), &handler) == handlers.end())
		{
			handlers.push_back(&handler);
		}
	}

	void EventBus::Unsubscribe(EventType type, Eventhandler& handler)
	{
		auto& handlers = m_EventHandlers[type];
		handlers.erase(std::remove(handlers.begin(), handlers.end(), &handler), handlers.end());
	}

	void EventBus::PublishEvent(const Event& event)
	{
		auto iter = m_EventHandlers.find(event.Type);
		if (iter == m_EventHandlers.end())
		{
			return;
		}

		auto handlers = iter->second;
		for (auto* handler : handlers)
		{
			if (!handler)
			{
				continue;
			}

			if (handler->ProcessEvent(event))
			{
				break;
			}
		}
	}

	void EventBus::EnqueueEvent(const Event& event)
	{
		m_EventQueue[event.Type].push_back(&event);
	}

	void EventBus::ProcessQueued()
	{
		for (auto& [type, queue] : m_EventQueue)
		{
			if (queue.empty())
			{
				continue;
			}

			auto iter = m_EventHandlers.find(type);
			if (iter == m_EventHandlers.end())
			{
				// If no handlers are present, discard the event.
				LUDUS_LOG_WARN(std::format("The event bus discarded an event, as no handlers of type {} was present.", type));
				queue.clear();

				continue;
			}

			auto handlers = iter->second;

			std::vector<const Event*> events;
			events.swap(queue);

			// Make sure that handled events break the handler loop.
			for (auto* event : events)
			{
				if (!event)
				{
					continue;
				}

				for (auto* handler : handlers)
				{
					if (!handler)
					{
						continue;
					}

					if (handler->ProcessEvent(*event))
					{
						break;
					}
				}
			}
		}

		// Make sure that the queue is cleared.
		std::erase_if(m_EventQueue, [](auto& pair) { return pair.second.empty(); });
	}

	int EventBus::GetHandlerCount(EventType type) const
	{
		auto iter = m_EventHandlers.find(type);
		if (iter == m_EventHandlers.end())
		{
			return 0;
		}

		return static_cast<int>(iter->second.size());
	}

	int EventBus::GetEventCount(EventType type) const
	{
		auto iter = m_EventQueue.find(type);
		if (iter == m_EventQueue.end())
		{
			return 0;
		}

		return static_cast<int>(iter->second.size());
	}
}
