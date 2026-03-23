#pragma once

#include <cstdint>

namespace Ludus::Engine::Runtime
{
	enum class SystemPhaseOrder : uint8_t { Before = 0, Normal = 1, After = 2 };
}
