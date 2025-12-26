#include "pch.h"

#include <cstdint>
#include <initializer_list>
#include <memory>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationOptions.h>
#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/FlagSet.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/ResourceRegistry.h>
#include <Ludus/Engine/Core/SceneManager.h>
#include <Ludus/Engine/Core/Scheduler.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Core/SystemDescriptor.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/Time.h>
#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Events/WindowEvents.h>
#include <Ludus/Engine/Graphics/GLContext.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Platform/Input.h>
#include <Ludus/Engine/Platform/Window.h>

namespace Ludus::Engine::Core
{
	Application::Application(
		Ludus::Engine::Core::ApplicationOptions applicationOptions,
		Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
		Ludus::Engine::Graphics::RenderingOptions renderingOptions,
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration,
		Ludus::Engine::Platform::WindowOptions windowOptions
	) : m_EntityComponentSystem(std::make_unique<Ludus::Engine::Core::EntityComponentSystem>()),
		m_EventBus(std::make_unique<Ludus::Engine::Events::EventBus>()),
		m_Input(std::make_unique<Ludus::Engine::Platform::Input>()),
		m_Window(std::make_unique<Ludus::Engine::Platform::Window>(windowOptions, *m_EventBus)),
		m_GLContext(std::make_unique<Ludus::Engine::Graphics::GLContext>()),
		m_RenderingConfiguration(std::make_unique<Ludus::Engine::Graphics::RenderingConfiguration2D>(std::move(renderingConfiguration))),
		m_PhysicsConfiguration(std::make_unique<Ludus::Engine::Physics::Core::PhysicsConfiguration2D>(std::move(physicsConfiguration))),
		m_Resources(std::make_unique<Ludus::Engine::Core::ResourceRegistry>()),
		m_RenderViewRequestRegistry(std::make_unique<Ludus::Engine::Core::RenderViewRequestRegistry>()),
		m_RenderViewRegistry(std::make_unique<Ludus::Engine::Core::RenderViewRegistry>()),
		m_SceneManager(std::make_unique<Ludus::Engine::Core::SceneManager>()),
		m_Time(std::make_unique<Ludus::Engine::Core::Time>()),
		m_ExecutionFlags(applicationOptions.ExecutionMask),
		m_SystemContext(
			*m_EntityComponentSystem,
			*m_EventBus,
			m_ExecutionFlags,
			*m_Input,
			*m_Resources,
			*m_RenderViewRegistry,
			*m_RenderViewRequestRegistry,
			*m_SceneManager,
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
		Ludus::Engine::Core::ApplicationOptions applicationOptions,
		Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration,
		Ludus::Engine::Graphics::RenderingOptions renderingOptions,
		Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration,
		Ludus::Engine::Platform::WindowOptions windowOptions
	)
	{
		auto application = std::make_unique<Application>(applicationOptions, std::move(renderingConfiguration), renderingOptions, std::move(physicsConfiguration), windowOptions);
		return application;
	}

	void Application::AddSystem(SystemDescriptor info, std::unique_ptr<ISystem> system)
	{
		system->OnAttach(m_SystemContext);
		m_Scheduler->AttachSystem(info, std::move(system));
	}

	void Application::AddSystem(std::initializer_list<SystemDescriptor> info, std::unique_ptr<ISystem> system)
	{
		system->OnAttach(m_SystemContext);
		m_Scheduler->AttachSystem(info, std::move(system));
	}

	void Application::Run()
	{
		while (!m_Window->WindowShouldClose())
		{
			const auto simulationEnabled = m_SystemContext.ExecutionFlags.HasAny(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled);
			m_Time->Step(simulationEnabled);

			m_Input->Clear();
			m_RenderViewRequestRegistry->Clear();
			m_RenderViewRegistry->Clear();

			m_Window->PollEvents();

			while (simulationEnabled && m_Time->ConsumeFixed())
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
