#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseInfo.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderingSystem2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsContext2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsSystem2D.h>
#include <Ludus/Engine/Platform/WindowOptions.h>

namespace Ludus::Engine::Core
{
	using BuilderCommand = std::function<void(Application&)>;

	class ApplicationBuilder
	{
	private:
		Ludus::Engine::Platform::WindowOptions m_WindowOptions;
		Ludus::Engine::Graphics::RenderingOptions m_RenderingOptions;
		Ludus::Engine::Physics::Core::PhysicsContext2D m_PhysicsContext;

		std::vector<BuilderCommand> m_BuilderCommands;

		bool m_HasDefaultRendering2D = false;
		bool m_HasDefaultPhysics2D = false;

	public:
		static ApplicationBuilder Create() { return ApplicationBuilder { }; }

		std::unique_ptr<Ludus::Engine::Core::Application> Build();

		ApplicationBuilder& WithWindowOptions(Ludus::Engine::Platform::WindowOptions windowOptions);
		ApplicationBuilder& WithRenderingOptions(Ludus::Engine::Graphics::RenderingOptions renderingOptions);
		ApplicationBuilder& WithPhysicsContext(Ludus::Engine::Physics::Core::PhysicsContext2D physicsContext);

		ApplicationBuilder& UseDefaultRendering2D();
		ApplicationBuilder& UseDefaultPhysics2D();

		ApplicationBuilder& Configure(BuilderCommand command);

#pragma region Templates

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddSystem(Ludus::Engine::Core::SystemPhaseInfo info, TArgs... args)
		{
			m_BuilderCommands.emplace_back(
				[=](Ludus::Engine::Core::Application& application)
				{
					application.AddSystem<TSystem>(info, args...);
				}
			);

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddFixedUpdateSystem(TArgs... args)
		{
			m_BuilderCommands.emplace_back(
				[=](Ludus::Engine::Core::Application& application)
				{
					application.AddFixedUpdateSystem<TSystem>(args...);
				}
			);

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddRenderSystem(TArgs... args)
		{
			m_BuilderCommands.emplace_back(
				[=](Ludus::Engine::Core::Application& application)
				{
					application.AddRenderSystem<TSystem>(args...);
				}
			);

			return *this;
		}

		template<typename TSystem, typename... TArgs>
		ApplicationBuilder& AddUpdateSystem(TArgs... args)
		{
			m_BuilderCommands.emplace_back(
				[=](Ludus::Engine::Core::Application& application)
				{
					application.AddUpdateSystem<TSystem>(args...);
				}
			);

			return *this;
		}

		template<typename T>
		ApplicationBuilder& AddResource(T resource)
		{
			m_BuilderCommands.emplace_back(
				[=](Ludus::Engine::Core::Application& application)
				{
					application.AddResource<T>(std::move(resource));
				}
			);

			return *this;
		}

#pragma endregion

	};
}
