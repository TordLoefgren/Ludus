#pragma once

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Scripting/LoadedScriptModule.h>
#include <Ludus/Engine/Scripting/ScriptEngine.h>
#include <Ludus/Engine/Scripting/ScriptInstanceState.h>

namespace Ludus::Engine::Core
{
	struct SceneRegistry;
}

namespace Ludus::Engine::Physics::Queries
{
	class IPhysicsQueryCache2D;
}

namespace Ludus::Engine::Runtime
{
	class IHostContext;
	struct RuntimeManifest;
	struct SceneRuntimeState;
}

namespace Ludus::Engine::Scripting
{
	class ScriptSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		const Ludus::Engine::Runtime::RuntimeManifest& m_RuntimeManifest;
		std::filesystem::path m_ScriptModulePath;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;
		std::unordered_map<Ludus::Engine::Core::ScriptId, const LoadedScriptDefinition*> m_DefinitionsById;
		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* m_QueryCache;
		ScriptEngine m_ScriptEngine;
		LoadedScriptModule m_LoadedScriptModule;

		std::vector<ScriptInstanceState> m_InstanceStates;
		std::uint64_t m_FrameCounter = 0;
		bool m_IsModuleLoaded = false;

		void BuildDefinitions();

		bool LoadModule();
		bool UnloadModule();

		ScriptInstanceState& FindOrCreateInstanceState(
			Ludus::Engine::Core::SceneId sceneId,
			Ludus::Engine::Core::EntityId ownerId
		);
		void DestroyInactiveInstances(std::uint64_t currentFrame);
		void DestroyInstance(const ScriptInstanceState& state);
		void DestroyAllInstances();

		void DispatchCollisionCallbacks(ScriptInstanceState& state);

	public:
		ScriptSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
			Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* queryCache,
			const std::filesystem::path& scriptModulePath
		);

	protected:
		virtual void OnTransitionEnterImpl() override;
		virtual void OnTransitionExitImpl() override;
		virtual void UpdateImpl(float deltaTime) override;
		virtual void OnDetachImpl() override;
	};
}
