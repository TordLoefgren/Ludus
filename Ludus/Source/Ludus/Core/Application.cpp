#include "Ludus/Core/Application.h"

namespace Ludus::Core
{
	Application::Application(
		Ludus::Platform::WindowOptions windowOptions,
		Ludus::Graphics::RenderingOptions renderingOptions,
		Ludus::Physics::Core::PhysicsContext2D physicsContext
	) : m_EventBus(std::make_unique<Ludus::Events::EventBus>()),
		m_Input(std::make_unique<Ludus::Platform::Input>()),
		m_Window(std::make_unique<Ludus::Platform::Window>(windowOptions, *m_EventBus)),
		m_GLContext(std::make_unique<Ludus::Graphics::GLContext>()),
		m_PhysicsContext(std::make_unique<Ludus::Physics::Core::PhysicsContext2D>(std::move(physicsContext))),
		m_Resources(std::make_unique<Ludus::Core::ResourceRegistry>()),
		m_EntityComponentSystem(std::make_unique<Ludus::Engine::EntityComponentSystem>()),
		m_Time(std::make_unique<Ludus::Engine::Time>()),
		m_SystemContext(
			*m_EntityComponentSystem,
			*m_EventBus,
			*m_Input,
			*m_Resources,
			*m_Window,
			m_PhysicsContext ? m_PhysicsContext->QueryCache.get() : nullptr
		),
		m_Scheduler(std::make_unique<Ludus::Core::Scheduler>(m_SystemContext))
	{
		m_GLContext->Init();
		m_GLContext->EnableBlending();
		m_GLContext->SetBlendAlpha();

		AddSystem(Phase::FixedUpdate, std::make_unique<Ludus::Physics::Core::PhysicsSystem2D>(*m_PhysicsContext));
		AddSystem(Phase::Render, std::make_unique<Ludus::Graphics::RenderingSystem2D>(renderingOptions));

		SubscribeToEvents();
	}

	std::unique_ptr<Application> Application::Create(Ludus::Platform::WindowOptions windowOptions, Ludus::Graphics::RenderingOptions renderingOptions)
	{
		auto application = std::make_unique<Application>(windowOptions, renderingOptions);
		return application;
	}

	void Application::AddSystem(Phase phase, std::unique_ptr<ISystem> system, SystemPredicate predicate)
	{
		system->OnAttach(m_SystemContext);
		m_Scheduler->AttachSystem(phase, std::move(system), predicate);
	}

	void Application::Run()
	{
		while (!m_Window->WindowShouldClose())
		{
			m_Time->Step();
			m_Input->Clear();

			m_Window->PollEvents();

			while (m_Time->ConsumeFixed())
			{
				m_Scheduler->Run(Phase::FixedUpdate, m_Time->GetFixed());
			}

			m_EventBus->ProcessQueued();

			m_Scheduler->ApplyResourceTransitions();

			m_Scheduler->UpdateTransitions();

			m_Scheduler->Run(Phase::Update, m_Time->GetSeconds());

			m_Scheduler->Run(Phase::Render);

			m_Window->SwapBuffers();
		}
	}

	void Application::SubscribeToEvents()
	{
		using EventType = Ludus::Events::EventType;

		m_EventBus->Subscribe(Events::EventType::KeyEvent, (Ludus::Events::Eventhandler&)*m_Input);
		m_EventBus->Subscribe(Events::EventType::TextInputEvent, (Ludus::Events::Eventhandler&)*m_Input);
		m_EventBus->Subscribe(Events::EventType::MouseButtonEvent, (Ludus::Events::Eventhandler&)*m_Input);
		m_EventBus->Subscribe(Events::EventType::MouseMoveEvent, (Ludus::Events::Eventhandler&)*m_Input);
		m_EventBus->Subscribe(Events::EventType::MouseScrollEvent, (Ludus::Events::Eventhandler&)*m_Input);
		m_EventBus->Subscribe(Events::EventType::WindowFocusEvent, (Ludus::Events::Eventhandler&)*m_Input);

		m_EventBus->Subscribe(Events::EventType::FramebufferSizeEvent, (Ludus::Events::Eventhandler&)*m_GLContext);

		m_EventBus->Subscribe(Events::EventType::WindowCloseEvent, *this);
	}

	bool Application::ProcessEvent(const Ludus::Events::Event& event)
	{
		using EventType = Ludus::Events::EventType;

		switch (event.Type)
		{
			case EventType::WindowCloseEvent:
			{
				m_Window->SetWindowShouldClose();
				return true;
			}
			case EventType::WindowIconifyEvent:
			{
				// Not implemented.
				return false;
			}

			default: return false;
		}
	}
}
