#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/Phase.h>

namespace Ludus::Core
{
	struct LoopScheduler
	{
	private:
		std::unordered_map<Phase, std::vector<std::unique_ptr<ISystem>>> m_SystemMap;

	public:
		void AttachSystem(Phase phase, std::unique_ptr<ISystem> system)
		{
			m_SystemMap[phase].push_back(std::move(system));
		}

		void Run(Phase phase, float time = 0.0f)
		{
			if (m_SystemMap.contains(phase))
			{
				for (auto& system : m_SystemMap[phase])
				{
					switch (phase)
					{
						case Phase::FixedUpdate:
							system->FixedUpdate(time);
							break;
						case Phase::Update:
							system->Update(time);
							break;
						default:
							break;
					}
				}
			}
		}
	};
}
