#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/Phase.h>
#include <Ludus/Core/ScheduledSystem.h>
#include <Ludus/Core/SystemPredicate.h>

namespace Ludus::Core
{
	struct Scheduler
	{
		SystemContext& m_SystemContext;

		explicit Scheduler(SystemContext& context) : m_SystemContext(context) { }

	private:
		std::unordered_map<Phase, std::vector<ScheduledSystem>> m_Systems;

	public:

		void AttachSystem(Phase phase, std::unique_ptr<ISystem> system, SystemPredicate predicate)
		{
			m_Systems[phase].push_back({ std::move(system), std::move(predicate) });
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
			for (auto& [phase, systems] : m_Systems)
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

		void Run(Phase phase, float time = 0.0f)
		{
			if (m_Systems.contains(phase))
			{
				for (auto& entry : m_Systems[phase])
				{
					if (!entry.IsActive)
					{
						continue;
					}

					switch (phase)
					{
						case Phase::FixedUpdate:
							entry.System->FixedUpdate(time);
							break;
						case Phase::Update:
							entry.System->Update(time);
							break;
						case Phase::Render:
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
