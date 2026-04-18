#pragma once

#include <memory>
#include <utility>

#include <Ludus/Engine/Core/Time.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Graphics/GLContext.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Persistence/EnginePersistence.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeOptions.h>
#include <Ludus/Engine/Runtime/SystemScheduler.h>
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Engine/Windowing/Window.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

#pragma region Forward Declarations

struct GLFWwindow;

namespace Ludus::Engine::Runtime
{
	struct SystemDescriptor;
	class RuntimeInstance;
	class ISystem;
}

#pragma endregion

namespace Ludus::Engine::Runtime
{
	class ApplicationHost : public Ludus::Engine::Events::EventHandler, public IHostContext
	{
	private:
		const Ludus::Engine::Persistence::EnginePersistence m_Persistence;
		Ludus::Engine::Events::EventBus m_EventBus;
		Ludus::Engine::Core::Enums::FlagSet m_ExecutionFlags;
		Ludus::Engine::Windowing::Input m_Input;
		Ludus::Engine::Core::Time m_Time;
		Ludus::Engine::Windowing::Window m_Window;
		Ludus::Engine::Graphics::GLContext m_GLContext;
		Ludus::Engine::Graphics::RenderTarget m_MainRenderTarget;
		SystemScheduler m_Scheduler;

		RuntimeInstance* m_Runtime;

		void SubscribeToEvents();
		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;

		ApplicationHost(
			Ludus::Engine::Persistence::EnginePersistence enginePersistence,
			Ludus::Engine::Runtime::RuntimeOptions runtimeOptions,
			Ludus::Engine::Windowing::WindowOptions windowOptions
		);

	public:
		~ApplicationHost();

		static std::unique_ptr<ApplicationHost> Create(
			Ludus::Engine::Persistence::EnginePersistence enginePersistence = Ludus::Engine::Persistence::EnginePersistence::DefaultText(),
			Ludus::Engine::Runtime::RuntimeOptions runtimeOptions = Ludus::Engine::Runtime::RuntimeOptions(),
			Ludus::Engine::Windowing::WindowOptions windowOptions = Ludus::Engine::Windowing::WindowOptions()
		);

		virtual void AttachRuntime(RuntimeInstance* runtime) override;
		virtual void DetachRuntime() override;

		void AddSystem(SystemDescriptor descriptor, std::unique_ptr<ISystem> system);
		void AddSystem(std::initializer_list<SystemDescriptor> descriptor, std::unique_ptr<ISystem> system);

		void Run();

#pragma region Host Context

		virtual Ludus::Engine::Math::Size<int> GetFramebufferSize() const override;

		virtual Ludus::Engine::Math::Size<int> GetWindowSize() const override;

		virtual GLFWwindow* GetWindowHandle() const override;

		virtual Ludus::Engine::Windowing::Input& GetInput() override;

		virtual const Ludus::Engine::Persistence::EnginePersistence& GetEnginePersistence() const override;

		virtual Ludus::Engine::Graphics::RenderTarget& GetMainRenderTarget() override;

		virtual Ludus::Engine::Core::Enums::FlagSet& GetExecutionFlags() override;

		virtual void SetWindowShouldClose() override;
		virtual void SetWindowTitle(std::string_view title) override;

		virtual void SubscribeWindowCloseEvent(Ludus::Engine::Events::EventHandler& handler) override;
		virtual void UnsubscribeWindowCloseEvent(Ludus::Engine::Events::EventHandler& handler) override;

#pragma endregion

#pragma region Templates

		template<typename TSystem, typename... TArgs>
		void AddSystem(Ludus::Engine::Runtime::SystemDescriptor descriptor, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(descriptor, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddSystem(std::initializer_list<Ludus::Engine::Runtime::SystemDescriptor> descriptors, TArgs&&... args)
		{
			auto system = std::make_unique<TSystem>(std::forward<TArgs>(args)...);
			AddSystem(descriptors, std::move(system));
		}

		template<typename TSystem, typename... TArgs>
		void AddBeginFrameSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::BeginFrame }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddFixedUpdateSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::FixedUpdate }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddRenderSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::Render }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddUpdateSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::Update }, std::forward<TArgs>(args)...);
		}

		template<typename TSystem, typename... TArgs>
		void AddEndFrameSystem(TArgs&&... args)
		{
			AddSystem<TSystem>({ Ludus::Engine::Runtime::SystemPhase::EndFrame }, std::forward<TArgs>(args)...);
		}

#pragma endregion

	};
}
