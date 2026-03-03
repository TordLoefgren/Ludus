#include "pch.h"

#include <utility>

#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	SystemContext::SystemContext(
		Ludus::Engine::Events::EventBus& events,
		Ludus::Engine::Core::FlagSet& executionFlags,
		Ludus::Engine::Windowing::Input& input,
		Ludus::Engine::Persistence::ProjectRepository& projectRepository,
		Ludus::Engine::Core::ResourceRegistry& resources,
		Ludus::Engine::Core::RenderViewRegistry& renderViews,
		Ludus::Engine::Core::RenderViewRequestRegistry& renderViewRequests,
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Windowing::Window& window,
		Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentation,
		Ludus::Engine::Core::ScriptRuntimeState scriptRuntime,
		std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> windowRenderTarget,
		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* queries
	) : EventBus(events),
		ExecutionFlags(executionFlags),
		Input(input),
		ProjectContext(std::nullopt),
		ProjectRepository(projectRepository),
		Resources(resources),
		RenderViewRequests(renderViewRequests),
		RenderViews(renderViews),
		SceneRegistry(sceneRegistry),
		Window(window),
		RenderPresentation(renderPresentation),
		ScriptRuntime(scriptRuntime),
		WindowRenderTarget(std::move(windowRenderTarget)),
		PhysicsQueries(queries)
	{ }

	bool SystemContext::HasProjectContext() const
	{
		return ProjectContext.has_value();
	}

	bool SystemContext::HasPhysics() const
	{
		return PhysicsQueries != nullptr;
	}

	Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D& SystemContext::GetPhysicsQueries()
	{
		LUDUS_ASSERT(PhysicsQueries, "Physics not enabled in this SystemContext.");
		return *PhysicsQueries;
	}
}
