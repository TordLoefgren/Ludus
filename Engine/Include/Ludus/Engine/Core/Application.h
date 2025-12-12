#pragma once

#include <initializer_list>
#include <memory>

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/ResourceRegistry.h>
#include <Ludus/Engine/Core/Scheduler.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseInfo.h>
#include <Ludus/Engine/Core/Time.h>
#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Events/WindowEvents.h>
#include <Ludus/Engine/Graphics/GLContext.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Physics/Core/PhysicsContext2D.h>
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
		std::unique_ptr<Ludus::Engine::Core::RenderViewRegistry> m_RenderViewRegistry;
		std::unique_ptr<Ludus::Engine::Core::EntityComponentSystem> m_EntityComponentSystem;
		std::unique_ptr<Ludus::Engine::Core::Time> m_Time;
		Ludus::Engine::Core::SystemContext m_SystemContext;
		std::unique_ptr<Ludus::Engine::Core::Scheduler> m_Scheduler;

	public:
		Application(
			Ludus::Engine::Platform::WindowOptions windowOptions = Ludus::Engine::Platform::WindowOptions(),
			Ludus::Engine::Graphics::RenderingOptions renderingOptions = Ludus::Engine::Graphics::RenderingOptions(),
			Ludus::Engine::Physics::Core::PhysicsContext2D physicsContext = Ludus::Engine::Physics::Core::PhysicsContext2D()
		);
		~Application() = default;

		static std::unique_ptr<Application> Create(
			Ludus::Engine::Platform::WindowOptions windowOptions = Ludus::Engine::Platform::WindowOptions(),
			Ludus::Engine::Graphics::RenderingOptions renderingOptions = Ludus::Engine::Graphics::RenderingOptions(),
			Ludus::Engine::Physics::Core::PhysicsContext2D physicsContext = Ludus::Engine::Physics::Core::PhysicsContext2D()
		);

		void AddSystem(SystemPhaseInfo info, std::unique_ptr<ISystem> system);
		void AddSystem(std::initializer_list<SystemPhaseInfo> info, std::unique_ptr<ISystem> system);

		void Run();

		SystemContext& GetSystemContext() { return m_SystemContext; }
		const SystemContext& GetSystemContext() const { return m_SystemContext; }

		Ludus::Engine::Physics::Core::PhysicsContext2D& GetPhysicsContext() { return *m_PhysicsContext; }
		const Ludus::Engine::Physics::Core::PhysicsContext2D& GetPhysicsContext() const { return *m_PhysicsContext; }

		void SubscribeToEvents();
		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;

#pragma region Templates

		template<typename T>
		void AddResource(T resource) { m_Resources->Add(std::move(resource)); }

		template<typename TSystem, typename... TArgs>
		void AddSystem(Ludus::Engine::Core::SystemPhaseInfo info, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(info, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddSystem(std::initializer_list<Ludus::Engine::Core::SystemPhaseInfo> infos, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(infos, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddFixedUpdateSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Core::SystemPhase::FixedUpdate }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddRenderSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Core::SystemPhase::Render }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddUpdateSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Core::SystemPhase::Update }, std::forward<TArgs>(args)...);
		}

#pragma endregion

	};
}
