#pragma once

#include <memory>
#include <optional>

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/FlagSet.h>
#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Core/ResourceRegistry.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Events/EventBus.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>             
#include <Ludus/Engine/Persistence/LmlProjectPersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>
#include <Ludus/Engine/Persistence/ProjectRepository.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>       
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Engine/Windowing/Window.h>

namespace Ludus::Engine::Core
{
	struct SystemContext
	{
		Ludus::Engine::Core::EntityComponentSystem& EntityComponentSystem;
		Ludus::Engine::Events::EventBus& EventBus;
		Ludus::Engine::Core::FlagSet& ExecutionFlags;
		Ludus::Engine::Windowing::Input& Input;
		std::optional<Ludus::Engine::Persistence::ProjectContext> ProjectContext;
		Ludus::Engine::Persistence::ProjectRepository& ProjectRepository;
		Ludus::Engine::Core::ResourceRegistry& Resources;
		Ludus::Engine::Core::RenderViewRequestRegistry& RenderViewRequests;
		Ludus::Engine::Core::RenderViewRegistry& RenderViews;
		Ludus::Engine::Core::SceneRegistry& SceneRegistry;
		Ludus::Engine::Windowing::Window& Window;
		Ludus::Engine::Graphics::RenderPresentationSettings& RenderPresentation;

		std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> WindowRenderTarget = nullptr;
		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* PhysicsQueries = nullptr;

		SystemContext(
			Ludus::Engine::Core::EntityComponentSystem& ecs,
			Ludus::Engine::Events::EventBus& events,
			Ludus::Engine::Core::FlagSet& executionFlags,
			Ludus::Engine::Windowing::Input& input,
			Ludus::Engine::Persistence::ProjectRepository& projectRepository,
			Ludus::Engine::Core::ResourceRegistry& resources,
			Ludus::Engine::Core::RenderViewRegistry& renderViews,
			Ludus::Engine::Core::RenderViewRequestRegistry& renderViewRequests,
			Ludus::Engine::Core::SceneRegistry& SceneRegistry,
			Ludus::Engine::Windowing::Window& window,
			Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentation,
			std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> windowRenderTarget = nullptr,
			Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* queries = nullptr
		) : EntityComponentSystem(ecs),
			EventBus(events),
			ExecutionFlags(executionFlags),
			Input(input),
			Resources(resources),
			ProjectContext(std::nullopt),
			ProjectRepository(projectRepository),
			RenderViewRequests(renderViewRequests),
			SceneRegistry(SceneRegistry),
			RenderViews(renderViews),
			Window(window),
			RenderPresentation(renderPresentation),
			WindowRenderTarget(windowRenderTarget),
			PhysicsQueries(queries)
		{ }

		bool HasProjectContext() const
		{
			return ProjectContext.has_value();
		}

		bool HasPhysics() const
		{
			return PhysicsQueries != nullptr;
		}

		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D& GetPhysicsQueries()
		{
			LUDUS_ASSERT(PhysicsQueries, "Physics not enabled in this SystemContext.");
			return *PhysicsQueries;
		}

		SystemContext(const SystemContext&) = delete;
		SystemContext& operator=(const SystemContext&) = delete;
		SystemContext(SystemContext&&) = delete;
		SystemContext& operator=(SystemContext&&) = delete;
	};
}
