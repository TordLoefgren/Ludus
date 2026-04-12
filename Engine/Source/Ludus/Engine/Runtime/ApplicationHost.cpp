#include "pch.h"

#include <Ludus/Engine/Events/WindowEvents.h>
#include <Ludus/Engine/Runtime/ApplicationHost.h>
#include <Ludus/Engine/Runtime/RuntimeInstance.h>

namespace Ludus::Engine::Runtime
{
	void ApplicationHost::SubscribeToEvents()
	{
		using EventType = Ludus::Engine::Events::EventType;

		auto& eventBus = m_EventBus;
		auto& input = m_Input;

		eventBus.Subscribe(EventType::KeyEvent, input);
		eventBus.Subscribe(EventType::TextInputEvent, input);
		eventBus.Subscribe(EventType::MouseButtonEvent, input);
		eventBus.Subscribe(EventType::MouseMoveEvent, input);
		eventBus.Subscribe(EventType::MouseScrollEvent, input);
		eventBus.Subscribe(EventType::WindowFocusEvent, input);
		eventBus.Subscribe(EventType::FramebufferSizeEvent, m_GLContext);
		eventBus.Subscribe(EventType::FramebufferSizeEvent, *this);
		eventBus.Subscribe(EventType::WindowCloseEvent, *this);
	}

	bool ApplicationHost::ProcessEvent(const Ludus::Engine::Events::Event& event)
	{
		using EventType = Ludus::Engine::Events::EventType;

		switch (event.Type)
		{
			case EventType::WindowCloseEvent:
			{
				if (m_EventBus.GetHandlerCount(EventType::WindowCloseEvent) == 1)
				{
					m_Window.SetWindowShouldClose();
					return true;
				}

				return false;
			}

			case EventType::FramebufferSizeEvent:
			{
				const auto& e = static_cast<const Ludus::Engine::Events::WindowEvents::FramebufferSizeEvent&>(event);
				m_MainRenderTarget.Framebuffer.Resize(e.Width, e.Height);

				return false;
			}

			default: return false;
		}
	}

	ApplicationHost::ApplicationHost(
		Ludus::Engine::Runtime::RuntimeOptions runtimeOptions,
		Ludus::Engine::Windowing::WindowOptions windowOptions
	) : m_EventBus(),
		m_ExecutionFlags(runtimeOptions.ExecutionMask),
		m_Input(),
		m_Time(),
		m_Window(windowOptions, m_EventBus),
		m_GLContext(),
		m_MainRenderTarget(
			Ludus::Engine::Graphics::RenderTarget::CreateDefault(
				m_Window.GetFramebufferSize().Width,
				m_Window.GetFramebufferSize().Height
			)
		),
		m_Scheduler(),
		m_Runtime(nullptr)
	{
		m_GLContext.Init();
		m_GLContext.EnableBlending();
		m_GLContext.SetBlendAlpha();
		const auto [framebufferWidth, framebufferHeight] = m_Window.GetFramebufferSize();
		m_GLContext.SetViewport(framebufferWidth, framebufferHeight);

		SubscribeToEvents();
	}

	ApplicationHost::~ApplicationHost() = default;

	std::unique_ptr<ApplicationHost> ApplicationHost::Create(
		Ludus::Engine::Runtime::RuntimeOptions runtimeOptions,
		Ludus::Engine::Windowing::WindowOptions windowOptions
	)
	{
		auto host = std::unique_ptr<ApplicationHost>(new ApplicationHost(runtimeOptions, windowOptions));
		return host;
	}

	void ApplicationHost::AttachRuntime(RuntimeInstance* runtime)
	{
		if (!runtime)
		{
			LUDUS_LOG_WARN("Attempted to attach a null runtime.");
			return;
		}

		runtime->Initialize();

		m_Runtime = runtime;
	}

	void ApplicationHost::DetachRuntime()
	{
		if (m_Runtime)
		{
			m_Runtime = nullptr;
		}
	}

	void ApplicationHost::AddSystem(SystemDescriptor info, std::unique_ptr<ISystem> system)
	{
		m_Scheduler.AddSystem(info, std::move(system));
	}

	void ApplicationHost::AddSystem(std::initializer_list<SystemDescriptor> info, std::unique_ptr<ISystem> system)
	{
		m_Scheduler.AddSystem(info, std::move(system));
	}

	void ApplicationHost::Run()
	{
		m_Scheduler.AttachSystems();

		while (!m_Window.WindowShouldClose())
		{
			const auto simulationEnabled = m_ExecutionFlags.HasAny(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled);
			m_Time.Step(simulationEnabled);

			m_Input.Clear();

			m_Scheduler.UpdateTransitions(m_ExecutionFlags);
			if (m_Runtime)
			{
				m_Runtime->UpdateTransitions(m_ExecutionFlags);
			}

			// Begin frame.
			m_Scheduler.Run(SystemPhase::BeginFrame, m_ExecutionFlags);
			if (m_Runtime)
			{
				m_Runtime->Run(SystemPhase::BeginFrame, m_ExecutionFlags);
			}

			m_Window.PollEvents();

			// Fixed update.
			while (simulationEnabled && m_Time.ConsumeFixed())
			{
				m_Scheduler.Run(SystemPhase::FixedUpdate, m_ExecutionFlags, m_Time.GetFixed());
				if (m_Runtime)
				{
					m_Runtime->Run(SystemPhase::FixedUpdate, m_ExecutionFlags, m_Time.GetFixed());
				}
			}

			m_EventBus.ProcessQueued();

			// Update.
			m_Scheduler.Run(SystemPhase::Update, m_ExecutionFlags, m_Time.GetSeconds());
			if (m_Runtime)
			{
				m_Runtime->Run(SystemPhase::Update, m_ExecutionFlags, m_Time.GetSeconds());
			}

			// Render.
			m_Scheduler.Run(SystemPhase::Render, m_ExecutionFlags);
			if (m_Runtime)
			{
				m_Runtime->Run(SystemPhase::Render, m_ExecutionFlags);
			}

			// End frame.
			m_Scheduler.Run(SystemPhase::EndFrame, m_ExecutionFlags);
			if (m_Runtime)
			{
				m_Runtime->Run(SystemPhase::EndFrame, m_ExecutionFlags);
			}

			m_Window.SwapBuffers();
		}

		m_Scheduler.DetachSystems();

		if (m_Runtime)
		{
			m_Runtime->Shutdown();
		}

		DetachRuntime();
	}

#pragma region Host Context

	Ludus::Engine::Math::Size<int> ApplicationHost::GetFramebufferSize() const
	{
		return m_Window.GetFramebufferSize();
	}

	Ludus::Engine::Math::Size<int> ApplicationHost::GetWindowSize() const
	{
		return m_Window.GetWindowSize();
	}

	GLFWwindow* ApplicationHost::GetWindowHandle() const
	{
		return m_Window.GetHandle();
	}

	Ludus::Engine::Windowing::Input& ApplicationHost::GetInput()
	{
		return m_Input;
	}

	Ludus::Engine::Graphics::RenderTarget& ApplicationHost::GetMainRenderTarget()
	{
		return m_MainRenderTarget;
	}

	Ludus::Engine::Core::Enums::FlagSet& ApplicationHost::GetExecutionFlags()
	{
		return m_ExecutionFlags;
	}

	void ApplicationHost::SetWindowShouldClose()
	{
		m_Window.SetWindowShouldClose();
	}

	void ApplicationHost::SetWindowTitle(std::string_view title)
	{
		m_Window.SetTitle(title);
	}

	void ApplicationHost::SubscribeWindowCloseEvent(Ludus::Engine::Events::EventHandler& handler)
	{
		m_EventBus.Subscribe(Ludus::Engine::Events::EventType::WindowCloseEvent, handler);
	}

	void ApplicationHost::UnsubscribeWindowCloseEvent(Ludus::Engine::Events::EventHandler& handler)
	{
		m_EventBus.Unsubscribe(Ludus::Engine::Events::EventType::WindowCloseEvent, handler);
	}

#pragma endregion

}
