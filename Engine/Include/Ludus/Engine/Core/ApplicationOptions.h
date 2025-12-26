#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/ExecutionFlags.h>

namespace Ludus::Engine::Core
{
	struct ApplicationOptions
	{
		uint32_t ExecutionMask = Ludus::Engine::Core::DefaultExecutionMask;
	};
}
