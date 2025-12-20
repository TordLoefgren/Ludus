#pragma once

#include <cstdint>

namespace Ludus::Engine::Graphics
{
	enum class RenderPhaseOrder : uint8_t { Before = 0, Normal = 1, After = 2 };
}
