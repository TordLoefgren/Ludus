#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Mask.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>
#include <Ludus/Engine/Scripting/API/Types.h>
#include <Ludus/Engine/Scripting/ScriptInstanceState.h>
#include <Ludus/Engine/Scripting/ScriptRepository.h>

namespace Ludus::Engine::Scripting
{
	class ScriptSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		ScriptRepository m_ScriptRepository;
		std::vector<ScriptInstanceState> m_InstanceStates;
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		const Ludus::Engine::Runtime::RuntimeEnvironment& m_RuntimeEnvironment;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;

		uint64_t m_FrameCounter = 0;
		bool m_IsModuleLoaded = false;

		void DestroyInstance(const ScriptInstanceState& state)
		{
			if (!state.HasCreated || !state.Definition || !state.Definition->OnDestroy)
			{
				return;
			}

			state.Definition->OnDestroy(state.OwnerHandle, &m_Context);
		}

		void DestroyAllInstances()
		{
			for (const auto& state : m_InstanceStates)
			{
				DestroyInstance(state);
			}

			m_InstanceStates.clear();
		}

		bool LoadModule()
		{
			if (m_IsModuleLoaded)
			{
				return false;
			}

			m_IsModuleLoaded = m_ScriptRepository.LoadScriptModule(
				m_RuntimeEnvironment.RuntimeRootDirectory,
				m_RuntimeEnvironment.ScriptModulePlatform,
				m_RuntimeEnvironment.ScriptModuleConfiguration
			);

			// The module might already have been loaded, in which case we return whether the module was loaded this frame.
			return m_IsModuleLoaded;
		}

		bool UnloadModule()
		{
			if (!m_IsModuleLoaded)
			{
				return false;
			}

			DestroyAllInstances();

			const auto success = m_ScriptRepository.UnloadScriptModule();
			m_IsModuleLoaded = false;

			// The module might already have been unloaded, in which case we return whether the module was unloaded this frame.
			return success;
		}

		static void OnDebugImpl(const char* message)
		{
			LUDUS_LOG_DEBUG(message ? message : "");
		}

		static void OnPrintImpl(const char* message)
		{
			LUDUS_LOG_INFO(message ? message : "");
		}

		Ludus::Engine::Scripting::API::ScriptContext m_Context
		{
			.Debug = &OnDebugImpl,
			.Print = &OnPrintImpl
		};

	public:
		ScriptSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			const Ludus::Engine::Runtime::RuntimeEnvironment& environment,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry
		) :
			m_HostContext(hostContext),
			m_RuntimeEnvironment(environment),
			m_SceneRegistry(sceneRegistry)
		{ }

		~ScriptSystem() = default;

	protected:
		virtual void OnTransitionEnterImpl() override
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

		virtual void OnTransitionExitImpl() override
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

		ScriptInstanceState& FindOrCreateInstanceState(Ludus::Engine::Core::SceneHandle sceneHandle, Ludus::Engine::Core::EntityHandle ownerHandle)
		{
			// Return existing.
			for (auto& state : m_InstanceStates)
			{
				if (state.SceneHandle == sceneHandle && state.OwnerHandle == ownerHandle)
				{
					return state;
				}
			}

			// Return new.
			m_InstanceStates.push_back(ScriptInstanceState
				{
					.SceneHandle = sceneHandle,
					.OwnerHandle = ownerHandle,
					.Definition = nullptr,
					.HasCreated = false,
					.LastSeenFrame = 0
				}
			);

			return m_InstanceStates.back();
		}

		void DestroyInactiveInstances(std::uint64_t currentFrame)
		{
			for (auto iter = m_InstanceStates.begin(); iter != m_InstanceStates.end(); )
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

		virtual void UpdateImpl(float deltaTime) override
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
				for (auto& script : scene.EntityComponentSystem.Scripts.ViewMutable())
				{
					const auto* definition = m_ScriptRepository.TryFindDefinition(script.Name);
					if (!definition)
					{
						continue;
					}

					auto& state = FindOrCreateInstanceState(scene.Handle, script.OwnerHandle);
					state.LastSeenFrame = currentFrame;

					// If the script has changed, destroy the old instance and reset its lifecycle.
					if (state.Definition != definition)
					{
						DestroyInstance(state);
						state.Definition = definition;
						state.HasCreated = false;
					}

					if (!state.HasCreated)
					{
						if (definition->OnCreate)
						{
							definition->OnCreate(script.OwnerHandle, &m_Context);
						}
						state.HasCreated = true;
					}

					if (definition->OnUpdate)
					{
						definition->OnUpdate(script.OwnerHandle, deltaTime, &m_Context);
					}
				}
			}

			// Destroy instances that have not been seen this frame.
			DestroyInactiveInstances(currentFrame);
		}

		virtual void OnDetachImpl() override
		{
			UnloadModule();
		}
	};
}
