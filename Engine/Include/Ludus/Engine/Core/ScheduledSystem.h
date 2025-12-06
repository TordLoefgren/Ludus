#pragma once

#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>
#include <Ludus/Engine/Core/SystemPredicate.h>

namespace Ludus::Engine::Core
{
	struct ScheduledSystem
	{
		ISystem* System;
		SystemPredicate Predicate;
		SystemPhaseOrder Order = SystemPhaseOrder::Normal;
		bool IsActive = false;
	};
}
