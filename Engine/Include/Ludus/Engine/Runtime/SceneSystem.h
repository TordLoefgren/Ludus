#pragma once

#include <memory>

#include <Ludus/Engine/Runtime/ISystem.h>

namespace Ludus::Engine::Core
{
	struct SceneRegistry;
}

namespace Ludus::Engine::Persistence
{
	class IScenePersistence;
}

namespace Ludus::Engine::Runtime
{
	struct SceneRuntimeState;
}

namespace Ludus::Engine::Runtime
{
	class SceneSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;
		Ludus::Engine::Runtime::SceneRuntimeState& m_SceneRuntimeState;
		std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> m_ScenePersistence;

	public:
		SceneSystem(
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState
		);

		SceneSystem(
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
			std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> scenePersistence
		);

		~SceneSystem();

	protected:
		virtual void BeginFrameImpl() override;
	};
}
