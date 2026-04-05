#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Scripting/LoadedScriptModule.h>
#include <Ludus/Engine/Scripting/ScriptEngine.h>
#include <Ludus/Engine/Scripting/ScriptInstanceState.h>

namespace Ludus::Engine::Scripting
{
	class ScriptSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		std::filesystem::path m_ScriptModulePath;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;
		std::vector<Ludus::Engine::Runtime::ScriptReference> m_ScriptReferences;
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
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			std::vector<Ludus::Engine::Runtime::ScriptReference> scriptReferences,
			Ludus::Engine::Core::SceneId& activeSceneId,
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
