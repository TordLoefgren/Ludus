#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Core/SystemPredicate.h>

namespace Ludus::Core
{
	struct ScheduledSystem
	{
		std::unique_ptr<ISystem>  System;
		SystemPredicate Predicate;
		bool IsActive = false;
	};
}
