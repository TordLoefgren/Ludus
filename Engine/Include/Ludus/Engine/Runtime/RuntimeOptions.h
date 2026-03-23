#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/ExecutionFlags.h>

namespace Ludus::Engine::Runtime
{
	struct RuntimeOptions
	{
		uint32_t ExecutionMask = Ludus::Engine::Core::DefaultExecutionMask;
	};
}
