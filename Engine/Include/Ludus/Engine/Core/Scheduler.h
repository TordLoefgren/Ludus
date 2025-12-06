#pragma once

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/ScheduledSystem.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseInfo.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>
#include <Ludus/Engine/Core/SystemPredicate.h>

namespace Ludus::Engine::Core
{
	struct Scheduler
	{
		SystemContext& m_SystemContext;

		explicit Scheduler(SystemContext& context) : m_SystemContext(context) { }

	private:
		std::vector<std::unique_ptr<ISystem>> m_Systems;
		std::unordered_map<SystemPhase, std::vector<ScheduledSystem>> m_SystemsByPhase;

		void SortPhase(SystemPhase phase)
		{
			auto& systems = m_SystemsByPhase[phase];
			std::sort(
				systems.begin(),
				systems.end(),
				[](const ScheduledSystem& a, const ScheduledSystem& b)
				{
					return a.Order < b.Order;
				}
			);
		}

	public:

		void AttachSystem(SystemPhaseInfo info, std::unique_ptr<ISystem> system)
		{
			auto* system_ptr = system.get();
			m_Systems.push_back(std::move(system));

			m_SystemsByPhase[info.Phase].push_back({ system_ptr, info.Predicate, info.Order });
			SortPhase(info.Phase);
		}

		void AttachSystem(std::initializer_list<SystemPhaseInfo> info, std::unique_ptr<ISystem> system)
		{
			auto* system_ptr = system.get();
			m_Systems.push_back(std::move(system));

			for (auto [phase, predicate, order] : info)
			{
				m_SystemsByPhase[phase].push_back({ system_ptr, predicate, order });
				SortPhase(phase);
			}
		}

		void ApplyResourceTransitions()
		{
			const auto transitionableResources = m_SystemContext.Resources.GetTransitionableResources();
			for (auto* resource : transitionableResources)
			{
				if (resource)
				{
					resource->ApplyStateTransition();
				}
			}
		}

		void UpdateTransitions()
		{
			for (auto& [phase, systems] : m_SystemsByPhase)
			{
				for (auto& entry : systems)
				{
					auto shouldBeActive = !entry.Predicate || entry.Predicate(m_SystemContext);
					if (!shouldBeActive && entry.IsActive)
					{
						entry.System->OnTransitionExit();
						entry.IsActive = false;
					}
				}

				for (auto& entry : systems)
				{
					auto shouldBeActive = !entry.Predicate || entry.Predicate(m_SystemContext);
					if (shouldBeActive && !entry.IsActive)
					{
						entry.System->OnTransitionEnter();
						entry.IsActive = true;
					}
				}
			}
		}

		void Run(SystemPhase phase, float time = 0.0f)
		{
			if (m_SystemsByPhase.contains(phase))
			{
				for (auto& entry : m_SystemsByPhase[phase])
				{
					if (!entry.IsActive)
					{
						continue;
					}

					switch (phase)
					{
						case SystemPhase::FixedUpdate:
							entry.System->FixedUpdate(time);
							break;
						case SystemPhase::Update:
							entry.System->Update(time);
							break;
						case SystemPhase::Render:
							entry.System->Render();
							break;
						default:
							break;
					}
				}
			}
		}
	};
}
