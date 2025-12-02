#pragma once

#include <Ludus/Core/SystemPhase.h>
#include <Ludus/Core/SystemPhaseOrder.h>

namespace Ludus::Core
{
	struct SystemPhaseInfo
	{
		SystemPhase Phase;
		SystemPredicate Predicate = nullptr;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
	};
}
