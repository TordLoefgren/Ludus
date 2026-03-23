#pragma once

#include <Ludus/Engine/Runtime/SystemConstraints.h>
#include <Ludus/Engine/Runtime/SystemPhase.h>
#include <Ludus/Engine/Runtime/SystemPhaseOrder.h>

namespace Ludus::Engine::Runtime
{
	struct SystemDescriptor
	{
		SystemPhase Phase;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
		SystemConstraints Constraints { };
	};
}
