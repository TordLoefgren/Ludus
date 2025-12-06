#pragma once

#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>

namespace Ludus::Engine::Core
{
	struct SystemPhaseInfo
	{
		SystemPhase Phase;
		SystemPredicate Predicate = nullptr;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
	};
}
