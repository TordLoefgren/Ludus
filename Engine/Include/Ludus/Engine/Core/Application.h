#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include <Ludus/Engine/Core/ApplicationOptions.h>
#include <Ludus/Engine/Core/FlagSet.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/ResourceRegistry.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Core/SystemDescriptor.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

#pragma region Forward Declarations

namespace Ludus::Engine::Core
{
	struct RenderViewRegistry;
	struct RenderViewRequestRegistry;
	struct SceneRegistry;
	struct Scheduler;
	struct Time;
}

namespace Ludus::Engine::Events
{
	struct Event;
	class EventBus;
}

namespace Ludus::Engine::Graphics
{
	class GLContext;
}

namespace Ludus::Engine::Persistence
{
	struct ProjectRepository;
}

namespace Ludus::Engine::Windowing
{
	class Input;
	class Window;
}

#pragma endregion

namespace Ludus::Engine::Core
{
	class Application : public Ludus::Engine::Events::EventHandler
	{
	private:
		std::unique_ptr<Ludus::Engine::Events::EventBus> m_EventBus;
		std::unique_ptr<Ludus::Engine::Windowing::Input> m_Input;
		std::unique_ptr<Ludus::Engine::Windowing::Window> m_Window;
		std::unique_ptr<Ludus::Engine::Graphics::GLContext> m_GLContext;
		std::unique_ptr<Ludus::Engine::Graphics::RenderPresentationSettings> m_RenderPresentationSettings;
		std::unique_ptr<Ludus::Engine::Graphics::RenderingConfiguration2D> m_RenderingConfiguration;
		std::unique_ptr<Ludus::Engine::Physics::Core::PhysicsConfiguration2D> m_PhysicsConfiguration;
		std::unique_ptr<Ludus::Engine::Core::ResourceRegistry> m_Resources;
		std::unique_ptr<Ludus::Engine::Core::RenderViewRegistry> m_RenderViewRegistry;
		std::unique_ptr<Ludus::Engine::Core::RenderViewRequestRegistry> m_RenderViewRequestRegistry;
		std::unique_ptr<Ludus::Engine::Core::SceneRegistry> m_SceneRegistry;
		std::unique_ptr<Ludus::Engine::Persistence::ProjectRepository> m_ProjectRepository;
		std::unique_ptr<Ludus::Engine::Core::Time> m_Time;
		Ludus::Engine::Core::FlagSet m_ExecutionFlags;
		Ludus::Engine::Core::SystemContext m_SystemContext;
		std::unique_ptr<Ludus::Engine::Core::Scheduler> m_Scheduler;

	public:
		Application(
			Ludus::Engine::Core::ApplicationOptions applicationOptions = Ludus::Engine::Core::ApplicationOptions(),
			Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration = Ludus::Engine::Graphics::RenderingConfiguration2D(),
			Ludus::Engine::Graphics::RenderingOptions renderingOptions = Ludus::Engine::Graphics::RenderingOptions(),
			Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings = Ludus::Engine::Graphics::RenderPresentationSettings(),
			Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration = Ludus::Engine::Physics::Core::PhysicsConfiguration2D(),
			Ludus::Engine::Windowing::WindowOptions windowOptions = Ludus::Engine::Windowing::WindowOptions()
		);
		~Application();

		static std::unique_ptr<Application> Create(
			Ludus::Engine::Core::ApplicationOptions applicationOptions = Ludus::Engine::Core::ApplicationOptions(),
			Ludus::Engine::Graphics::RenderingConfiguration2D renderingConfiguration = Ludus::Engine::Graphics::RenderingConfiguration2D(),
			Ludus::Engine::Graphics::RenderingOptions renderingOptions = Ludus::Engine::Graphics::RenderingOptions(),
			Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings = Ludus::Engine::Graphics::RenderPresentationSettings(),
			Ludus::Engine::Physics::Core::PhysicsConfiguration2D physicsConfiguration = Ludus::Engine::Physics::Core::PhysicsConfiguration2D(),
			Ludus::Engine::Windowing::WindowOptions windowOptions = Ludus::Engine::Windowing::WindowOptions()
		);

		void AddSystem(SystemDescriptor descriptor, std::unique_ptr<ISystem> system);
		void AddSystem(std::initializer_list<SystemDescriptor> descriptor, std::unique_ptr<ISystem> system);

		void Run();

		SystemContext& GetSystemContext() { return m_SystemContext; }
		const SystemContext& GetSystemContext() const { return m_SystemContext; }

		Ludus::Engine::Physics::Core::PhysicsConfiguration2D& GetPhysicsConfiguration() { return *m_PhysicsConfiguration; }
		const Ludus::Engine::Physics::Core::PhysicsConfiguration2D& GetPhysicsConfiguration() const { return *m_PhysicsConfiguration; }

		Ludus::Engine::Graphics::RenderingConfiguration2D& GetRenderingConfiguration() { return *m_RenderingConfiguration; }
		const Ludus::Engine::Graphics::RenderingConfiguration2D& GetRenderingConfiguration() const { return *m_RenderingConfiguration; }

		Ludus::Engine::Graphics::RenderPresentationSettings& GetRenderPresentationSettings() { return *m_RenderPresentationSettings; }
		const Ludus::Engine::Graphics::RenderPresentationSettings& GetRenderPresentationSettings() const { return *m_RenderPresentationSettings; }

		void SubscribeToEvents();
		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;

#pragma region Templates

		template<typename T>
		void AddResource(T resource) { m_Resources->Add(std::move(resource)); }

		template<typename T>
		T& GetResource() { return m_Resources->Get<T>(); }

		template<typename T>
		const T& GetResource() const { return m_Resources->Get<T>(); }

		template<typename TSystem, typename... TArgs>
		void AddSystem(Ludus::Engine::Core::SystemDescriptor descriptor, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(descriptor, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddSystem(std::initializer_list<Ludus::Engine::Core::SystemDescriptor> descriptors, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(descriptors, std::move(system));
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
