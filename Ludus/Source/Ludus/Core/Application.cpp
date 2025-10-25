#include "Ludus/Core/Application.h"

namespace Ludus::Core
{
	Application::Application(
		Ludus::Platform::WindowOptions options
	) : m_Input(std::make_unique<Ludus::Platform::Input>()),
		m_Window(std::make_unique<Ludus::Platform::Window>(options, *m_Input)),
		m_Scheduler(std::make_unique<Ludus::Core::LoopScheduler>()),
		m_RenderingSystem(std::make_unique<Ludus::Graphics::RenderingSystem2D>()),
		m_GLContext(std::make_unique<Ludus::Graphics::GLContext>()),
		m_EntityComponentSystem(std::make_unique<Ludus::Engine::EntityComponentSystem>()),
		m_Time(std::make_unique<Ludus::Engine::Time>()),
		m_SystemContext(*m_EntityComponentSystem, *m_Input)
	{
		m_GLContext->Init();
		m_GLContext->EnableBlending();
		m_GLContext->SetBlendAlpha();
	}

	std::unique_ptr<Application> Application::CreateApplication()
	{
		auto application = std::make_unique<Application>();

		application->AttachSystem(Phase::FixedUpdate, std::make_unique<Ludus::Physics::PhysicsSystem2D>());
		application->AttachSystem(Phase::Update, std::make_unique<Ludus::Engine::SceneSystem2D>());

		return application;
	}

	void Application::AttachSystem(Phase phase, std::unique_ptr<ISystem> system)
	{
		system->OnAttach(m_SystemContext);
		m_Scheduler->AttachSystem(phase, std::move(system));
	}

	void Application::Run()
	{
		while (!m_Window->WindowShouldClose())
		{
			m_Time->Step();

			m_Window->PollEvents();

			while (m_Time->ConsumeFixed())
			{
				m_Scheduler->Run(Phase::FixedUpdate, m_Time->GetFixed());
			}

			m_Scheduler->Run(Phase::Update, m_Time->GetSeconds());

			m_RenderingSystem->Run(*m_EntityComponentSystem);

			m_Window->SwapBuffers();
		}
	}
}
