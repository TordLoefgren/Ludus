#pragma once

#include <memory>
#include <optional>

#include <Ludus/Engine/Core/ProjectContext.h>

#pragma region Forward Declarations

namespace Ludus::Engine::Core
{
	struct FlagSet;
	struct RenderViewRegistry;
	struct RenderViewRequestRegistry;
	struct ResourceRegistry;
	struct SceneRegistry;
}

namespace Ludus::Engine::Events
{
	class EventBus;
}

namespace Ludus::Engine::Graphics
{
	struct RenderPresentationSettings;
	struct RenderTarget;
}

namespace Ludus::Engine::Persistence
{
	struct ProjectRepository;
}

namespace Ludus::Engine::Physics::Queries
{
	class IPhysicsQueryCache2D;
}

namespace Ludus::Engine::Windowing
{
	class Input;
	class Window;
}

#pragma endregion

namespace Ludus::Engine::Core
{
	struct SystemContext
	{
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
		);

		bool HasProjectContext() const;

		bool HasPhysics() const;

		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D& GetPhysicsQueries();

		SystemContext(const SystemContext&) = delete;
		SystemContext& operator=(const SystemContext&) = delete;
		SystemContext(SystemContext&&) = delete;
		SystemContext& operator=(SystemContext&&) = delete;
	};
}
