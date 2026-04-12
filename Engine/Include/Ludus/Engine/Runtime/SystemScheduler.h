#pragma once

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Enums/FlagSet.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/SystemDescriptor.h>
#include <Ludus/Engine/Runtime/SystemPhase.h>

namespace Ludus::Engine::Runtime
{
	struct SystemScheduler
	{
	private:
		struct ScheduledSystem
		{
			ISystem* System;
			SystemPhaseOrder Order = SystemPhaseOrder::Normal;
			SystemConstraints Constraints;
		};

		std::vector<std::unique_ptr<ISystem>> m_Systems;
		std::unordered_map<SystemPhase, std::vector<ScheduledSystem>> m_SystemsByPhase;
		std::unordered_map<ISystem*, std::vector<SystemConstraints>> m_ConstraintsBySystem;
		std::unordered_map<ISystem*, bool> m_SystemIsActive;
		bool m_AreSystemsAttached = false;

		static bool SatisfiesConstraints(
			const SystemConstraints& constraints,
			const Ludus::Engine::Core::Enums::FlagSet& executionFlags
		)
		{
			if (executionFlags.HasAny(constraints.ForbidAny))
			{
				return false;
			}

			if (!executionFlags.HasAll(constraints.RequireAll))
			{
				return false;
			}

			if (constraints.RequireAny != 0 && !executionFlags.HasAny(constraints.RequireAny))
			{
				return false;
			}

			return true;
		}

		void SortPhase(SystemPhase phase)
		{
			auto& systems = m_SystemsByPhase[phase];
			std::sort(systems.begin(), systems.end(), [](const ScheduledSystem& a, const ScheduledSystem& b)
			{
				return a.Order < b.Order;
			});
		}

		void RegisterSystem(
			ISystem* system,
			SystemPhase phase,
			SystemPhaseOrder order,
			SystemConstraints constraints
		)
		{
			m_SystemsByPhase[phase].push_back({ system, order, constraints });
			m_ConstraintsBySystem[system].push_back(constraints);
			SortPhase(phase);
		}

		bool ShouldBeActive(ISystem* system, const Ludus::Engine::Core::Enums::FlagSet& executionFlags) const
		{
			const auto iter = m_ConstraintsBySystem.find(system);
			if (iter == m_ConstraintsBySystem.end())
			{
				return false;
			}

			for (const auto& constraints : iter->second)
			{
				if (SatisfiesConstraints(constraints, executionFlags))
				{
					return true;
				}
			}

			return false;
		}

	public:
		void AddSystem(SystemDescriptor descriptor, std::unique_ptr<ISystem> system)
		{
			LUDUS_ASSERT(!m_AreSystemsAttached, "Cannot add systems after the scheduler has attached systems.");

			auto* system_ptr = system.get();
			m_Systems.push_back(std::move(system));
			m_SystemIsActive.emplace(system_ptr, false);

			RegisterSystem(system_ptr, descriptor.Phase, descriptor.Order, descriptor.Constraints);
		}

		void AddSystem(std::initializer_list<SystemDescriptor> descriptors, std::unique_ptr<ISystem> system)
		{
			LUDUS_ASSERT(!m_AreSystemsAttached, "Cannot add systems after the scheduler has attached systems.");

			auto* system_ptr = system.get();
			m_Systems.push_back(std::move(system));
			m_SystemIsActive.emplace(system_ptr, false);

			for (auto [phase, order, constraints] : descriptors)
			{
				RegisterSystem(system_ptr, phase, order, constraints);
			}
		}

		void UpdateTransitions(const Ludus::Engine::Core::Enums::FlagSet& executionFlags)
		{
			std::vector<ISystem*> systemsToDeactivate;
			std::vector<ISystem*> systemsToActivate;

			for (auto& [system, isActive] : m_SystemIsActive)
			{
				const bool shouldBeActive = ShouldBeActive(system, executionFlags);

				if (!shouldBeActive && isActive)
				{
					systemsToDeactivate.push_back(system);
				}

				if (shouldBeActive && !isActive)
				{
					systemsToActivate.push_back(system);
				}
			}

			for (auto* system : systemsToDeactivate)
			{
				system->OnTransitionExit();
				m_SystemIsActive[system] = false;
			}

			for (auto* system : systemsToActivate)
			{
				system->OnTransitionEnter();
				m_SystemIsActive[system] = true;
			}
		}

		void Run(SystemPhase phase, Ludus::Engine::Core::Enums::FlagSet& executionFlags, float time = 0.0f)
		{
			if (m_SystemsByPhase.contains(phase))
			{
				for (auto& entry : m_SystemsByPhase[phase])
				{
					if (!SatisfiesConstraints(entry.Constraints, executionFlags))
					{
						continue;
					}

					switch (phase)
					{
						case SystemPhase::BeginFrame:
							entry.System->BeginFrame();
							break;
						case SystemPhase::FixedUpdate:
							entry.System->FixedUpdate(time);
							break;
						case SystemPhase::Update:
							entry.System->Update(time);
							break;
						case SystemPhase::Render:
							entry.System->Render();
							break;
						case SystemPhase::EndFrame:
							entry.System->EndFrame();
							break;
						default:
							break;
					}
				}
			}
		}

		void AttachSystems()
		{
			if (m_AreSystemsAttached)
			{
				return;
			}

			for (auto& system : m_Systems)
			{
				system->OnAttach();
			}

			m_AreSystemsAttached = true;
		}

		void DetachSystems()
		{
			if (!m_AreSystemsAttached)
			{
				return;
			}

			for (auto& system : m_Systems)
			{
				system->OnDetach();
			}

			m_AreSystemsAttached = false;
		}
	};
}
