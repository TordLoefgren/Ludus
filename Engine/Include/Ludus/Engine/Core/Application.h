#pragma once

#include <initializer_list>
#include <memory>
#include <string>

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/ImGuiSystem.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/ResourceRegistry.h>
#include <Ludus/Engine/Core/Scheduler.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseInfo.h>
#include <Ludus/Engine/Core/Time.h>
#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Graphics/GLContext.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderingSystem2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Physics/Core/PhysicsContext2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsSystem2D.h>
#include <Ludus/Engine/Platform/Input.h>
#include <Ludus/Engine/Platform/Window.h>

namespace Ludus::Engine::Core
{
	class Application : public Ludus::Engine::Events::Eventhandler
	{
	private:
		std::unique_ptr<Ludus::Engine::Events::EventBus> m_EventBus;
		std::unique_ptr<Ludus::Engine::Platform::Input> m_Input;
		std::unique_ptr<Ludus::Engine::Platform::Window> m_Window;
		std::unique_ptr<Ludus::Engine::Graphics::GLContext> m_GLContext;
		std::unique_ptr<Ludus::Engine::Physics::Core::PhysicsContext2D> m_PhysicsContext;
		std::unique_ptr<Ludus::Engine::Core::ResourceRegistry> m_Resources;
		std::unique_ptr<Ludus::Engine::Core::EntityComponentSystem> m_EntityComponentSystem;
		std::unique_ptr<Ludus::Engine::Core::Time> m_Time;
		Ludus::Engine::Core::SystemContext m_SystemContext;
		std::unique_ptr<Ludus::Engine::Core::Scheduler> m_Scheduler;

		void RegisterDefaults(Ludus::Engine::Platform::WindowOptions windowOptions, Ludus::Engine::Graphics::RenderingOptions renderingOptions);

	public:
		Application(
			Ludus::Engine::Platform::WindowOptions windowOptions = Ludus::Engine::Platform::WindowOptions(),
			Ludus::Engine::Graphics::RenderingOptions renderingOptions = Ludus::Engine::Graphics::RenderingOptions(),
			Ludus::Engine::Physics::Core::PhysicsContext2D physicsContext = Ludus::Engine::Physics::Core::PhysicsContext2D()
		);
		~Application() = default;

		static std::unique_ptr<Application> Create(
			Ludus::Engine::Platform::WindowOptions windowOptions = Ludus::Engine::Platform::WindowOptions(),
			Ludus::Engine::Graphics::RenderingOptions renderingOptions = Ludus::Engine::Graphics::RenderingOptions()
		);

		template<typename T>
		void AddResource(T resource);

		void AddSystem(SystemPhaseInfo info, std::unique_ptr<ISystem> system);
		void AddSystem(std::initializer_list<SystemPhaseInfo> info, std::unique_ptr<ISystem> system);
		void Run();

		void SubscribeToEvents();
		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;
	};

	template<typename T>
	inline void Application::AddResource(T resource) { m_Resources->Add(std::move(resource)); }
}
