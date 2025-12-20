#include "pch.h"

#include <initializer_list>
#include <memory>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>

namespace Ludus::Engine::Core
{
	Application::Application(
		Ludus::Engine::Platform::WindowOptions windowOptions,
		Ludus::Engine::Graphics::RenderingOptions renderingOptions,
		Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration
	) : m_EntityComponentSystem(std::make_unique<Ludus::Engine::Core::EntityComponentSystem>()),
		m_EventBus(std::make_unique<Ludus::Engine::Events::EventBus>()),
		m_Input(std::make_unique<Ludus::Engine::Platform::Input>()),
		m_Window(std::make_unique<Ludus::Engine::Platform::Window>(windowOptions, *m_EventBus)),
		m_GLContext(std::make_unique<Ludus::Engine::Graphics::GLContext>()),
		m_RenderingConfiguration(std::make_unique<Ludus::Engine::Graphics::RenderingConfiguration2D>(std::move(renderingConfiguration))),
		m_PhysicsConfiguration(std::make_unique<Ludus::Engine::Physics::Core::PhysicsConfiguration2D>(std::move(physicsConfiguration))),
		m_Resources(std::make_unique<Ludus::Engine::Core::ResourceRegistry>()),
		m_RenderViewRegistry(std::make_unique<Ludus::Engine::Core::RenderViewRegistry>()),
		m_Time(std::make_unique<Ludus::Engine::Core::Time>()),
		m_SystemContext(
			*m_EntityComponentSystem,
			*m_EventBus,
			*m_Input,
			*m_Resources,
			*m_RenderViewRegistry,
			*m_Window,
			std::make_shared<Ludus::Engine::Graphics::RenderTarget>(windowOptions.Width, windowOptions.Height),
			m_PhysicsConfiguration ? m_PhysicsConfiguration->QueryCache.get() : nullptr
		),
		m_Scheduler(std::make_unique<Ludus::Engine::Core::Scheduler>(m_SystemContext))
	{
		m_GLContext->Init();
		m_GLContext->EnableBlending();
		m_GLContext->SetBlendAlpha();
		m_GLContext->SetViewport(windowOptions.Width, windowOptions.Height);

		SubscribeToEvents();
	}

	std::unique_ptr<Application> Application::Create(
		Ludus::Engine::Platform::WindowOptions windowOptions,
		Ludus::Engine::Graphics::RenderingOptions renderingOptions,
		Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration
	)
	{
		auto application = std::make_unique<Application>(windowOptions, renderingOptions, std::move(renderingConfiguration), std::move(physicsConfiguration));
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
			m_RenderViewRegistry->Clear();

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
		using EventType = Ludus::Engine::Events::EventType;
		using Eventhandler = Ludus::Engine::Events::Eventhandler;

		m_EventBus->Subscribe(EventType::KeyEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::TextInputEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::MouseButtonEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::MouseMoveEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::MouseScrollEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::WindowFocusEvent, (Eventhandler&)*m_Input);
		m_EventBus->Subscribe(EventType::FramebufferSizeEvent, *this);
		m_EventBus->Subscribe(EventType::FramebufferSizeEvent, (Eventhandler&)*m_GLContext);
		m_EventBus->Subscribe(EventType::WindowCloseEvent, *this);
	}

	bool Application::ProcessEvent(const Ludus::Engine::Events::Event& event)
	{
		using EventType = Ludus::Engine::Events::EventType;

		switch (event.Type)
		{
			case EventType::WindowCloseEvent:
			{
				m_Window->SetWindowShouldClose();

				return true;
			}
			case EventType::FramebufferSizeEvent:
			{
				const auto& e = static_cast<const Ludus::Engine::Events::WindowEvents::FramebufferSizeEvent&>(event);
				if (m_SystemContext.WindowRenderTarget)
				{
					m_SystemContext.WindowRenderTarget->Framebuffer.Resize(e.Width, e.Height);
				}

				// Allow GLContext to update as well.
				return false;
			}

			default: return false;
		}
	}
}
