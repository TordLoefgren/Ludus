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
		m_GLContext->SetViewport(windowOptions.Width, windowOptions.Height);

		RegisterDefaults(windowOptions, renderingOptions);

		SubscribeToEvents();
	}

	void Application::RegisterDefaults(Ludus::Platform::WindowOptions windowOptions, Ludus::Graphics::RenderingOptions renderingOptions)
	{
		AddSystem({ { SystemPhase::Update, nullptr, SystemPhaseOrder::Before }, { SystemPhase::Render, nullptr, SystemPhaseOrder::After} }, std::make_unique<Ludus::Editor::Core::ImGuiSystem>());
		AddSystem({ SystemPhase::FixedUpdate, nullptr, SystemPhaseOrder::Before }, std::make_unique<Ludus::Physics::Core::PhysicsSystem2D>(*m_PhysicsContext));
		AddSystem({ SystemPhase::Render, nullptr, SystemPhaseOrder::Before }, std::make_unique<Ludus::Graphics::RenderingSystem2D>(renderingOptions));

		AddResource<std::shared_ptr<Ludus::Graphics::RenderTarget>>(std::make_shared<Ludus::Graphics::RenderTarget>(windowOptions.Width, windowOptions.Height));
	}

	std::unique_ptr<Application> Application::Create(Ludus::Platform::WindowOptions windowOptions, Ludus::Graphics::RenderingOptions renderingOptions)
	{
		auto application = std::make_unique<Application>(windowOptions, renderingOptions);
		return application;
	}

	void Application::AddSystem(SystemPhaseInfo info, std::unique_ptr<ISystem> system)
	{
		system->OnAttach(m_SystemContext);
		m_Scheduler->AttachSystem(info, std::move(system));
	}

	void Application::AddSystem(std::initializer_list<SystemPhaseInfo> info, std::unique_ptr<ISystem> system)
	{
		system->OnAttach(m_SystemContext);
		m_Scheduler->AttachSystem(info, std::move(system));
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
				m_Scheduler->Run(SystemPhase::FixedUpdate, m_Time->GetFixed());
			}

			m_EventBus->ProcessQueued();

			m_Scheduler->ApplyResourceTransitions();

			m_Scheduler->UpdateTransitions();

			m_Scheduler->Run(SystemPhase::Update, m_Time->GetSeconds());

			m_Scheduler->Run(SystemPhase::Render);

			m_Window->SwapBuffers();
		}
	}

	void Application::SubscribeToEvents()
	{
		using EventType = Ludus::Events::EventType;
		using Eventhandler = Ludus::Events::Eventhandler;

		m_EventBus->Subscribe(EventType::KeyEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::TextInputEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::MouseButtonEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::MouseMoveEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::MouseScrollEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::WindowFocusEvent, (Eventhandler&)*m_Input);

		m_EventBus->Subscribe(EventType::FramebufferSizeEvent, (Eventhandler&)*m_GLContext);

		m_EventBus->Subscribe(EventType::WindowCloseEvent, *this);
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
