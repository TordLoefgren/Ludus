#include "pch.h"

#include <unordered_map>

#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Scripting/ScriptSystem.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	void ScriptSystem::DestroyInstance(const ScriptInstanceState& state)
	{
		if (!state.HasCreated || !state.Definition || !state.Definition->OnDestroy)
		{
			return;
		}

		state.Definition->OnDestroy(m_ScriptEngine.GetContext(), state.OwnerId.Value);
	}

	void ScriptSystem::DestroyAllInstances()
	{
		for (const auto& state : m_InstanceStates)
		{
			DestroyInstance(state);
		}

		m_InstanceStates.clear();
	}

	void ScriptSystem::DispatchCollisionCallbacks(ScriptInstanceState& state)
	{
		if (!m_QueryCache || !state.Definition)
		{
			state.ActiveCollisions.clear();
			return;
		}

		const bool hasCollisionCallbacks =
			state.Definition->OnCollisionEnter ||
			state.Definition->OnCollisionStay ||
			state.Definition->OnCollisionExit;
		if (!hasCollisionCallbacks)
		{
			state.ActiveCollisions.clear();
			return;
		}

		std::unordered_map<Ludus::Engine::Core::EntityId, Ludus::Scripting::ABI::Collision2DData> currentCollisions;

		for (const auto& contact : m_QueryCache->GetContacts(state.OwnerId))
		{
			const auto selfIsA = contact.EntityIdA == state.OwnerId;
			const auto otherId = selfIsA ? contact.EntityIdB : contact.EntityIdA;

			Ludus::Scripting::ABI::Collision2DData collision
			{
				.Other = otherId.Value,
				.LocalSelf = {
					selfIsA ? contact.Point.LocalA.X : contact.Point.LocalB.X,
					selfIsA ? contact.Point.LocalA.Y : contact.Point.LocalB.Y
			},
				.LocalOther = {
					selfIsA ? contact.Point.LocalB.X : contact.Point.LocalA.X,
					selfIsA ? contact.Point.LocalB.Y : contact.Point.LocalA.Y
			},
				.Normal = {
					selfIsA ? contact.Point.Normal.X : -contact.Point.Normal.X,
					selfIsA ? contact.Point.Normal.Y : -contact.Point.Normal.Y
			},
				.Penetration = contact.Point.Penetration,
				.IsTrigger = contact.IsTriggerPair
			};

			currentCollisions[otherId] = collision;
		}

		auto context = m_ScriptEngine.GetContext();

		for (const auto& [otherId, collision] : currentCollisions)
		{
			if (!state.ActiveCollisions.contains(otherId))
			{
				if (state.Definition->OnCollisionEnter)
				{
					state.Definition->OnCollisionEnter(context, state.OwnerId.Value, &collision);
				}

				continue;
			}

			if (state.Definition->OnCollisionStay)
			{
				state.Definition->OnCollisionStay(context, state.OwnerId.Value, &collision);
			}
		}

		for (const auto& [otherId, collision] : state.ActiveCollisions)
		{
			if (currentCollisions.contains(otherId))
			{
				continue;
			}

			if (state.Definition->OnCollisionExit)
			{
				state.Definition->OnCollisionExit(context, state.OwnerId.Value, &collision);
			}
		}

		state.ActiveCollisions = std::move(currentCollisions);
	}

	void ScriptSystem::BuildDefinitions()
	{
		m_DefinitionsById.clear();
		m_DefinitionsById.reserve(m_RuntimeManifest.Scripts.size());

		for (const auto& scriptReference : m_RuntimeManifest.Scripts)
		{
			const auto* definition = m_LoadedScriptModule.TryFindDefinition(scriptReference.Name);
			if (!definition)
			{
				continue;
			}

			m_DefinitionsById.emplace(scriptReference.Id, definition);
		}
	}

	bool ScriptSystem::LoadModule()
	{
		if (m_IsModuleLoaded)
		{
			return false;
		}

		m_IsModuleLoaded = m_LoadedScriptModule.LoadScriptModule(m_ScriptModulePath);
		if (m_IsModuleLoaded)
		{
			BuildDefinitions();
		}

		return m_IsModuleLoaded;
	}

	bool ScriptSystem::UnloadModule()
	{
		if (!m_IsModuleLoaded)
		{
			return false;
		}

		DestroyAllInstances();

		const auto success = m_LoadedScriptModule.UnloadScriptModule();
		m_DefinitionsById.clear();
		m_IsModuleLoaded = false;
		return success;
	}

	ScriptInstanceState& ScriptSystem::FindOrCreateInstanceState(
		Ludus::Engine::Core::SceneId sceneId,
		Ludus::Engine::Core::EntityId ownerId
	)
	{
		for (auto& state : m_InstanceStates)
		{
			if (state.SceneId == sceneId && state.OwnerId == ownerId)
			{
				return state;
			}
		}

		m_InstanceStates.push_back(ScriptInstanceState
			{
				.SceneId = sceneId,
				.OwnerId = ownerId,
				.Definition = nullptr,
				.HasCreated = false,
				.ActiveCollisions = { },
				.LastSeenFrame = 0
			}
		);

		return m_InstanceStates.back();
	}

	void ScriptSystem::DestroyInactiveInstances(std::uint64_t currentFrame)
	{
		for (auto iter = m_InstanceStates.begin(); iter != m_InstanceStates.end();)
		{
			if (iter->LastSeenFrame != currentFrame)
			{
				DestroyInstance(*iter);
				iter = m_InstanceStates.erase(iter);
				continue;
			}

			++iter;
		}
	}

	ScriptSystem::ScriptSystem(
		Ludus::Engine::Runtime::IHostContext& hostContext,
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest,
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* queryCache,
		const std::filesystem::path& scriptModulePath
	) :
		m_HostContext(hostContext),
		m_SceneRegistry(sceneRegistry),
		m_RuntimeManifest(runtimeManifest),
		m_ScriptEngine(runtimeManifest, m_SceneRegistry, sceneRuntimeState, m_HostContext.GetInput()),
		m_QueryCache(queryCache),
		m_ScriptModulePath(scriptModulePath)
	{ }

	void ScriptSystem::OnTransitionEnterImpl()
	{
		(void)UnloadModule();
		const auto isLoaded = LoadModule();
		if (isLoaded)
		{
			LUDUS_LOG_INFO("Script module loaded.");
		}
		else
		{
			LUDUS_LOG_WARN("Script module could not be loaded.");
		}
	}

	void ScriptSystem::OnTransitionExitImpl()
	{
		const auto isUnloaded = UnloadModule();
		if (isUnloaded)
		{
			LUDUS_LOG_INFO("Script module unloaded.");
		}
		else
		{
			LUDUS_LOG_WARN("Script module could not be unloaded.");
		}
	}

	void ScriptSystem::UpdateImpl(float deltaTime)
	{
		if (!m_IsModuleLoaded)
		{
			return;
		}

		if (!m_HostContext.GetExecutionFlags().HasAny(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled))
		{
			return;
		}

		const auto currentFrame = ++m_FrameCounter;

		for (auto& scene : m_SceneRegistry.ViewMutable())
		{
			m_ScriptEngine.SetContextScene(scene.Id);
			auto* context = m_ScriptEngine.GetContext();

			for (auto& script : scene.EntityComponentSystem.Scripts.ViewMutable())
			{
				const auto iter = m_DefinitionsById.find(script.Id);
				if (iter == m_DefinitionsById.end())
				{
					continue;
				}

				const auto* definition = iter->second;
				if (!definition)
				{
					continue;
				}

				auto& state = FindOrCreateInstanceState(scene.Id, script.OwnerId);
				state.LastSeenFrame = currentFrame;

				if (state.Definition != definition)
				{
					DestroyInstance(state);
					state.Definition = definition;
					state.HasCreated = false;
					state.ActiveCollisions.clear();
				}

				if (!state.HasCreated)
				{
					if (definition->OnCreate)
					{
						definition->OnCreate(context, script.OwnerId.Value);
					}
					state.HasCreated = true;
				}

				DispatchCollisionCallbacks(state);

				if (definition->OnUpdate)
				{
					definition->OnUpdate(context, script.OwnerId.Value, deltaTime);
				}
			}
		}

		DestroyInactiveInstances(currentFrame);
	}

	void ScriptSystem::OnDetachImpl()
	{
		UnloadModule();
	}
}
