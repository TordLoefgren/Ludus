#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include <Ludus/Engine/Runtime/ApplicationHost.h>
#include <Ludus/Engine/Runtime/RuntimeOptions.h>
#include <Ludus/Engine/Runtime/SystemDescriptor.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

namespace Ludus::Engine::Runtime
{
	using ApplicationHostBuilderCommand = std::function<void(ApplicationHost&)>;

	class ApplicationHostBuilder
	{
	private:
		Ludus::Engine::Runtime::RuntimeOptions m_RuntimeOptions;
		Ludus::Engine::Windowing::WindowOptions m_WindowOptions;

		std::vector<ApplicationHostBuilderCommand> m_BuilderCommands;

	public:
		static ApplicationHostBuilder Create() { return ApplicationHostBuilder { }; }

		std::unique_ptr<Ludus::Engine::Runtime::ApplicationHost> Build();

		ApplicationHostBuilder& WithRuntimeOptions(Ludus::Engine::Runtime::RuntimeOptions runtimeOptions);
		ApplicationHostBuilder& WithWindowOptions(Ludus::Engine::Windowing::WindowOptions windowOptions);

		ApplicationHostBuilder& Configure(ApplicationHostBuilderCommand command);

#pragma region Templates

		template<typename TSystem, typename... TArgs>
		ApplicationHostBuilder& AddSystem(Ludus::Engine::Runtime::SystemDescriptor descriptor, TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([descriptor, capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::ApplicationHost& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddSystem<TSystem>(descriptor, unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationHostBuilder& AddBeginFrameSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::ApplicationHost& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddBeginFrameSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationHostBuilder& AddFixedUpdateSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::ApplicationHost& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddFixedUpdateSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationHostBuilder& AddRenderSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::ApplicationHost& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddRenderSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationHostBuilder& AddUpdateSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::ApplicationHost& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddUpdateSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationHostBuilder& AddEndFrameSystem(TArgs&&... args)
		{
			auto capturedArgs = std::make_tuple(std::forward<TArgs>(args)...);

			m_BuilderCommands.emplace_back([capturedArgs = std::move(capturedArgs)](Ludus::Engine::Runtime::ApplicationHost& application) mutable
			{
				std::apply([&](auto&... unpackedArgs)
				{
					application.AddEndFrameSystem<TSystem>(unpackedArgs...);
				}, capturedArgs);
			});

			return *this;
		}

#pragma endregion

	};
}
