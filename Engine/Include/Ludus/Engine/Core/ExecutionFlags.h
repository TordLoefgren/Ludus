#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Mask.h>

namespace Ludus::Engine::Core
{
	enum class ExecutionFlags : uint32_t
	{
		None = 0,
		PhysicsEnabled = 1 << 0,
		RenderingEnabled = 1 << 1,
		ScriptingEnabled = 1 << 2,
		SimulationEnabled = 1 << 3
	};

	constexpr uint32_t DefaultExecutionMask =
		Ludus::Engine::Core::Mask(ExecutionFlags::None) |
		Ludus::Engine::Core::Mask(ExecutionFlags::PhysicsEnabled) |
		Ludus::Engine::Core::Mask(ExecutionFlags::RenderingEnabled) |
		Ludus::Engine::Core::Mask(ExecutionFlags::ScriptingEnabled) |
		Ludus::Engine::Core::Mask(ExecutionFlags::SimulationEnabled);
}
