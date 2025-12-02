#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/SystemPhaseOrder.h>
#include <Ludus/Core/SystemPredicate.h>

namespace Ludus::Core
{
	struct ScheduledSystem
	{
		ISystem* System;
		SystemPredicate Predicate;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
		bool IsActive = false;
	};
}
