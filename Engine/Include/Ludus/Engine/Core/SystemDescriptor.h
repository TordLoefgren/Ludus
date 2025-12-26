#pragma once

#include <Ludus/Engine/Core/SystemConstraints.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>

namespace Ludus::Engine::Core
{
	struct SystemDescriptor
	{
		SystemPhase Phase;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
		SystemConstraints Constraints{ };
		SystemPredicate Predicate = nullptr;
	};
}
