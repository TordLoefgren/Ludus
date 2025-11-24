#pragma once

#include <memory>
#include <string>

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/Phase.h>
#include <Ludus/Core/ResourceRegistry.h>
#include <Ludus/Core/Scheduler.h>
#include <Ludus/Core/SystemContext.h>
#include <Ludus/Core/SystemPredicate.h>
#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Engine/Time.h>
#include <Ludus/Events/Event.h>
#include <Ludus/Events/EventBus.h>
#include <Ludus/Events/EventHandler.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/RenderingOptions.h>
#include <Ludus/Graphics/RenderingSystem2D.h>
#include <Ludus/Physics/Core/PhysicsContext2D.h>
#include <Ludus/Physics/Core/PhysicsSystem2D.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>

namespace Ludus::Core
{
	class Application : public Ludus::Events::Eventhandler
	{
	private:
		std::unique_ptr<Ludus::Events::EventBus> m_EventBus;
		std::unique_ptr<Ludus::Platform::Input> m_Input;
		std::unique_ptr<Ludus::Platform::Window> m_Window;
		std::unique_ptr<Ludus::Graphics::GLContext> m_GLContext;
		std::unique_ptr<Ludus::Physics::Core::PhysicsContext2D> m_PhysicsContext;
		std::unique_ptr<Ludus::Core::ResourceRegistry> m_Resources;
		std::unique_ptr<Ludus::Engine::EntityComponentSystem> m_EntityComponentSystem;
		std::unique_ptr<Ludus::Engine::Time> m_Time;
		Ludus::Core::SystemContext m_SystemContext;
		std::unique_ptr<Ludus::Core::Scheduler> m_Scheduler;

	public:
		Application(
			Ludus::Platform::WindowOptions windowOptions = Ludus::Platform::WindowOptions(),
			Ludus::Graphics::RenderingOptions renderingOptions = Ludus::Graphics::RenderingOptions(),
			Ludus::Physics::Core::PhysicsContext2D physicsContext = Ludus::Physics::Core::PhysicsContext2D()
		);
		~Application() = default;

		static std::unique_ptr<Application> Create(
			Ludus::Platform::WindowOptions windowOptions = Ludus::Platform::WindowOptions(),
			Ludus::Graphics::RenderingOptions renderingOptions = Ludus::Graphics::RenderingOptions()
		);

		template<typename T>
		void AddResource(T resource);

		void AddSystem(Phase phase, std::unique_ptr<ISystem> system, SystemPredicate predicate = nullptr);
		void Run();

		void SubscribeToEvents();
		virtual bool ProcessEvent(const Ludus::Events::Event& event) override;
	};

	template<typename T>
	inline void Application::AddResource(T resource) { m_Resources->Add(std::move(resource)); }
}

