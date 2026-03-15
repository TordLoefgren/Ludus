#include "pch.h"

#include <vector>

namespace Ludus::EngineTests::Events
{
	using Ludus::Engine::Events::EventBus;
	using Ludus::Engine::Events::EventHandler;
	using Ludus::Engine::Events::Event;
	using Ludus::Engine::Events::EventType;

	struct ConcreteEventA final : Event
	{
		ConcreteEventA() : Event(EventType::MouseEnterEvent) { }
	};

	struct ConcreteEventB final : Event
	{
		ConcreteEventB() : Event(EventType::MouseButtonEvent) { }
	};

	struct NumberedEvent final : Event
	{
		int Id;

		explicit NumberedEvent(int id) : Event(EventType::MouseEnterEvent), Id(id) { }
	};

	struct ConcreteHandler final : EventHandler
	{
		int Counter = 0;
		bool HandleEvent;

		explicit ConcreteHandler(bool handleEvent = false) : HandleEvent(handleEvent) { }

		virtual bool ProcessEvent(const Event& event) override { Counter++; return HandleEvent; }
	};

	struct LoggingHandler final : EventHandler
	{
		int Id;
		std::vector<int>& Log;
		bool HandleEvent = false;

		explicit LoggingHandler(int id, std::vector<int>& log, bool handleEvent = false)
			: Id(id), Log(log), HandleEvent(handleEvent)
		{ }

		virtual bool ProcessEvent(const Event& event) override
		{
			if (auto* numberedEvent = dynamic_cast<const NumberedEvent*>(&event))
			{
				Log.push_back(numberedEvent->Id);
			}
			else
			{
				Log.push_back(Id);
			}

			return HandleEvent;
		}
	};

	TEST(EventBus, PublishEvent_Should_InvokeAllHandlers_When_NoHandlerConsumesEvent)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler1 = ConcreteHandler();
		auto handler2 = ConcreteHandler();
		auto handler3 = ConcreteHandler();

		bus.Subscribe(event.Type, handler1);
		bus.Subscribe(event.Type, handler2);
		bus.Subscribe(event.Type, handler3);

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(handler1.Counter, 1);
		EXPECT_EQ(handler2.Counter, 1);
		EXPECT_EQ(handler3.Counter, 1);
	}

	TEST(EventBus, PublishEvent_Should_StopInvokingHandlers_When_HandlerConsumesEvent)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler1 = ConcreteHandler(true);
		auto handler2 = ConcreteHandler();
		auto handler3 = ConcreteHandler();

		bus.Subscribe(event.Type, handler1);
		bus.Subscribe(event.Type, handler2);
		bus.Subscribe(event.Type, handler3);

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(handler1.Counter, 1);
		EXPECT_EQ(handler2.Counter, 0);
		EXPECT_EQ(handler3.Counter, 0);
	}

	TEST(EventBus, PublishEvent_Should_InvokeHandlersInOrderOfSubscription)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();

		auto log = std::vector<int>();
		auto handler1 = LoggingHandler(1, log);
		auto handler2 = LoggingHandler(2, log);
		auto handler3 = LoggingHandler(3, log);

		bus.Subscribe(event.Type, handler1);
		bus.Subscribe(event.Type, handler2);
		bus.Subscribe(event.Type, handler3);

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(log[0], 1);
		EXPECT_EQ(log[1], 2);
		EXPECT_EQ(log[2], 3);
	}

	TEST(EventBus, PublishEvent_Should_NotInvokeHandlers_When_NoHandlersSubscribed)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler = ConcreteHandler();

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(handler.Counter, 0);
	}

	TEST(EventBus, PublishEvent_Should_InvokeOnlyHandlersOfMatchingType_When_MultipleTypesSubscribed)
	{
		// Arrange.
		auto bus = EventBus();
		auto eventA = ConcreteEventA();
		auto eventB = ConcreteEventB();
		auto handler1 = ConcreteHandler();
		auto handler2 = ConcreteHandler();
		auto handler3 = ConcreteHandler();
		auto handler4 = ConcreteHandler();

		bus.Subscribe(eventA.Type, handler1);
		bus.Subscribe(eventA.Type, handler2);
		bus.Subscribe(eventB.Type, handler3);
		bus.Subscribe(eventB.Type, handler4);

		// Act.
		bus.PublishEvent(eventA);

		// Assert.
		EXPECT_EQ(handler1.Counter, 1);
		EXPECT_EQ(handler2.Counter, 1);
		EXPECT_EQ(handler3.Counter, 0);
		EXPECT_EQ(handler4.Counter, 0);
	}

	TEST(EventBus, Subscribe_Should_AllowSameHandlerToSubscribeToDifferentEventTypes)
	{
		// Arrange.
		auto bus = EventBus();
		auto eventA = ConcreteEventA();
		auto eventB = ConcreteEventB();
		auto handler = ConcreteHandler();

		bus.Subscribe(eventA.Type, handler);
		bus.Subscribe(eventB.Type, handler);

		// Act.
		bus.PublishEvent(eventA);
		bus.PublishEvent(eventB);

		// Assert.
		EXPECT_EQ(handler.Counter, 2);
	}

	TEST(EventBus, Subscribe_ShouldNot_RegisterDuplicateHandler_When_AlreadySubscribed)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler = ConcreteHandler();

		bus.Subscribe(event.Type, handler);
		bus.Subscribe(event.Type, handler);

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(handler.Counter, 1);
	}

	TEST(EventBus, Unsubscribe_Should_NotInvokeHandler_When_HandlerUnsubscribed)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler = ConcreteHandler();

		bus.Subscribe(event.Type, handler);
		bus.Unsubscribe(event.Type, handler);

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(handler.Counter, 0);
	}

	TEST(EventBus, Unsubscribe_Should_NotAffectOtherHandlers_When_RemovingOneHandler)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler1 = ConcreteHandler();
		auto handler2 = ConcreteHandler();
		auto handler3 = ConcreteHandler();

		bus.Subscribe(event.Type, handler1);
		bus.Subscribe(event.Type, handler2);
		bus.Subscribe(event.Type, handler3);
		bus.Unsubscribe(event.Type, handler3);

		// Act.
		bus.PublishEvent(event);

		// Assert.
		EXPECT_EQ(handler1.Counter, 1);
		EXPECT_EQ(handler2.Counter, 1);
		EXPECT_EQ(handler3.Counter, 0);
	}

	TEST(EventBus, Unsubscribe_Should_NotRemoveHandlerFromOtherEventTypes_When_SubscribedToMultiple)
	{
		// Arrange.
		auto bus = EventBus();
		auto eventA = ConcreteEventA();
		auto eventB = ConcreteEventB();
		auto handlerA = ConcreteHandler();
		auto handlerB = ConcreteHandler();

		bus.Subscribe(eventA.Type, handlerA);
		bus.Subscribe(eventB.Type, handlerB);
		bus.Unsubscribe(eventB.Type, handlerB);

		// Act.
		bus.PublishEvent(eventA);

		// Assert.
		EXPECT_EQ(handlerA.Counter, 1);
		EXPECT_EQ(handlerB.Counter, 0);
	}

	TEST(EventBus, Unsubscribe_ShouldNot_Throw_When_HandlerNotRegistered)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler = ConcreteHandler();

		// Act && Assert.
		EXPECT_NO_THROW(bus.Unsubscribe(event.Type, handler));
	}

	TEST(EventBus, EnqueueEvent_Should_StoreEventInQueue_When_Called)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();

		// Act.
		bus.EnqueueEvent(event);
		auto count = bus.GetEventCount(event.Type);

		// Assert.
		EXPECT_EQ(count, 1);
	}

	TEST(EventBus, ProcessQueued_Should_InvokeHandlersForQueuedEvents)
	{
		// Arrange.
		auto bus = EventBus();
		auto event1 = ConcreteEventA();
		auto event2 = ConcreteEventA();
		auto handler1 = ConcreteHandler();
		auto handler2 = ConcreteHandler();

		bus.Subscribe(event1.Type, handler1);
		bus.Subscribe(event2.Type, handler2);

		bus.EnqueueEvent(event1);
		bus.EnqueueEvent(event2);

		// Act.
		bus.ProcessQueued();

		// Assert.
		EXPECT_EQ(handler1.Counter, 2);
		EXPECT_EQ(handler2.Counter, 2);
	}

	TEST(EventBus, ProcessQueued_Should_ClearQueueAfterProcessing)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler = ConcreteHandler();

		bus.Subscribe(event.Type, handler);
		bus.EnqueueEvent(event);

		// Act.
		bus.ProcessQueued();
		auto count = bus.GetEventCount(event.Type);

		// Assert.
		EXPECT_EQ(count, 0);
	}

	TEST(EventBus, ProcessQueued_Should_InvokeHandlersInOrderOfEventEnqueue)
	{
		// Arrange.
		auto bus = EventBus();
		auto event1 = NumberedEvent(1);
		auto event2 = NumberedEvent(2);

		auto log = std::vector<int>();
		auto handler1 = LoggingHandler(0, log);
		auto handler2 = LoggingHandler(0, log);

		bus.Subscribe(event1.Type, handler1);
		bus.Subscribe(event1.Type, handler2);

		bus.EnqueueEvent(event1);
		bus.EnqueueEvent(event2);

		// Act.
		bus.ProcessQueued();

		// Assert.
		EXPECT_EQ(log[0], 1);
		EXPECT_EQ(log[1], 1);
		EXPECT_EQ(log[2], 2);
		EXPECT_EQ(log[3], 2);
	}

	TEST(EventBus, ProcessQueued_Should_StopProcessingHandlers_When_HandlerConsumesEvent)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler1 = ConcreteHandler(true);
		auto handler2 = ConcreteHandler();
		auto handler3 = ConcreteHandler();

		bus.Subscribe(event.Type, handler1);
		bus.Subscribe(event.Type, handler2);
		bus.Subscribe(event.Type, handler3);

		bus.EnqueueEvent(event);

		// Act.
		bus.ProcessQueued();

		// Assert.
		EXPECT_EQ(handler1.Counter, 1);
		EXPECT_EQ(handler2.Counter, 0);
		EXPECT_EQ(handler3.Counter, 0);
	}

	TEST(EventBus, ProcessQueued_Should_NotInvokeHandlers_When_QueueIsEmpty)
	{
		// Arrange.
		auto bus = EventBus();
		auto event = ConcreteEventA();
		auto handler1 = ConcreteHandler();
		auto handler2 = ConcreteHandler();

		bus.Subscribe(event.Type, handler1);
		bus.Subscribe(event.Type, handler2);

		// Act.
		bus.ProcessQueued();

		// Assert.
		EXPECT_EQ(handler1.Counter, 0);
		EXPECT_EQ(handler2.Counter, 0);
	}

	TEST(EventBus, ProcessQueued_Should_InvokeOnlyHandlersOfMatchingType_When_QueueContainsMultipleTypes)
	{
		// Arrange.
		auto bus = EventBus();
		auto eventA = ConcreteEventA();
		auto eventB = ConcreteEventB();
		auto handler1 = ConcreteHandler();
		auto handler2 = ConcreteHandler();

		bus.Subscribe(eventA.Type, handler1);
		bus.Subscribe(eventA.Type, handler2);

		bus.EnqueueEvent(eventA);
		bus.EnqueueEvent(eventB);

		// Act.
		bus.ProcessQueued();

		// Assert.
		EXPECT_EQ(handler1.Counter, 1);
		EXPECT_EQ(handler2.Counter, 1);
	}
}
