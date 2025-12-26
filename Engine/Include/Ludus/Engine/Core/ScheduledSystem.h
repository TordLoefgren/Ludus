#pragma once

#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/SystemConstraints.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>
#include <Ludus/Engine/Core/SystemPredicate.h>

namespace Ludus::Engine::Core
{
	struct ScheduledSystem
	{
		ISystem* System;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
		SystemConstraints Constraints;
		SystemPredicate Predicate;
		bool IsActive = false;
	};
}
