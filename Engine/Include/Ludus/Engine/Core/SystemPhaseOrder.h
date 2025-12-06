#pragma once

#include <cstdint>

namespace Ludus::Engine::Core
{
	enum class SystemPhaseOrder : uint8_t { Before = 0, Normal = 1, After = 2 };
}
