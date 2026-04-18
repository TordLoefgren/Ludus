#pragma once

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
		const Ludus::Engine::Persistence::IScenePersistence& m_ScenePersistence;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;
		Ludus::Engine::Runtime::SceneRuntimeState& m_SceneRuntimeState;

	public:
		SceneSystem(
			const Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState
		);

		~SceneSystem();

	protected:
		virtual void BeginFrameImpl() override;
	};
}
