#include "pch.h"

#include <unordered_map>

#include <Ludus/Engine/Core/Mask.h>
#include <Ludus/Engine/Debug/Debug.h>
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

		state.Definition->OnDestroy(m_ScriptEngine.GetContext(), state.OwnerHandle);
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

		std::unordered_map<Ludus::Engine::Core::EntityHandle, Ludus::Scripting::ABI::Collision2DData> currentCollisions;

		for (const auto& contact : m_QueryCache->GetContacts(state.OwnerHandle))
		{
			const auto selfIsA = contact.EntityHandleA == state.OwnerHandle;
			const auto otherHandle = selfIsA ? contact.EntityHandleB : contact.EntityHandleA;

			Ludus::Scripting::ABI::Collision2DData collision
			{
				.Other = otherHandle,
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

			currentCollisions[otherHandle] = collision;
		}

		auto context = m_ScriptEngine.GetContext();

		for (const auto& [otherHandle, collision] : currentCollisions)
		{
			if (!state.ActiveCollisions.contains(otherHandle))
			{
				if (state.Definition->OnCollisionEnter)
				{
					state.Definition->OnCollisionEnter(context, state.OwnerHandle, &collision);
				}

				continue;
			}

			if (state.Definition->OnCollisionStay)
			{
				state.Definition->OnCollisionStay(context, state.OwnerHandle, &collision);
			}
		}

		for (const auto& [otherHandle, collision] : state.ActiveCollisions)
		{
			if (currentCollisions.contains(otherHandle))
			{
				continue;
			}

			if (state.Definition->OnCollisionExit)
			{
				state.Definition->OnCollisionExit(context, state.OwnerHandle, &collision);
			}
		}

		state.ActiveCollisions = std::move(currentCollisions);
	}

	bool ScriptSystem::LoadModule()
	{
		if (m_IsModuleLoaded)
		{
			return false;
		}

		m_IsModuleLoaded = m_LoadedScriptModule.LoadScriptModule(m_ScriptModulePath);
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
		m_IsModuleLoaded = false;
		return success;
	}

	ScriptInstanceState& ScriptSystem::FindOrCreateInstanceState(
		Ludus::Engine::Core::SceneHandle sceneHandle,
		Ludus::Engine::Core::EntityHandle ownerHandle
	)
	{
		for (auto& state : m_InstanceStates)
		{
			if (state.SceneHandle == sceneHandle && state.OwnerHandle == ownerHandle)
			{
				return state;
			}
		}

		m_InstanceStates.push_back(ScriptInstanceState
			{
				.SceneHandle = sceneHandle,
				.OwnerHandle = ownerHandle,
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
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Core::SceneHandle& activeSceneHandle,
		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* queryCache,
		const std::filesystem::path& scriptModulePath
	) :
		m_HostContext(hostContext),
		m_SceneRegistry(sceneRegistry),
		m_ScriptEngine(m_SceneRegistry, m_HostContext.GetInput(), activeSceneHandle),
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
			m_ScriptEngine.SetActiveScene(scene.Handle);
			auto* context = m_ScriptEngine.GetContext();

			for (auto& script : scene.EntityComponentSystem.Scripts.ViewMutable())
			{
				const auto* definition = m_LoadedScriptModule.TryFindDefinition(script.Name);
				if (!definition)
				{
					continue;
				}

				auto& state = FindOrCreateInstanceState(scene.Handle, script.OwnerHandle);
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
						definition->OnCreate(context, script.OwnerHandle);
					}
					state.HasCreated = true;
				}

				DispatchCollisionCallbacks(state);

				if (definition->OnUpdate)
				{
					definition->OnUpdate(context, script.OwnerHandle, deltaTime);
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
