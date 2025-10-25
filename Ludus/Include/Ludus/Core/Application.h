#pragma once

#include <memory>

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/LoopScheduler.h>
#include <Ludus/Core/Phase.h>
#include <Ludus/Core/SystemContext.h>
#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Engine/SceneSystem2D.h>
#include <Ludus/Engine/Time.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/RenderingSystem2D.h>
#include <Ludus/Physics/PhysicsSystem2D.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>

namespace Ludus::Core
{
	class Application
	{
	private:
		std::unique_ptr<Ludus::Platform::Input> m_Input;
		std::unique_ptr<Ludus::Platform::Window> m_Window;
		std::unique_ptr<Ludus::Graphics::GLContext> m_GLContext;
		std::unique_ptr<Ludus::Graphics::RenderingSystem2D> m_RenderingSystem;
		std::unique_ptr<Ludus::Core::LoopScheduler> m_Scheduler;
		std::unique_ptr<Ludus::Engine::EntityComponentSystem> m_EntityComponentSystem;
		std::unique_ptr<Ludus::Engine::Time> m_Time;
		Ludus::Core::SystemContext m_SystemContext;

	public:
		Application(Ludus::Platform::WindowOptions options = Ludus::Platform::WindowOptions());
		~Application() = default;

		static std::unique_ptr <Application> CreateApplication();

		void AttachSystem(Phase phase, std::unique_ptr<ISystem> system);
		void Run();
	};
}

