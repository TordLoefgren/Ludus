#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Enums/EnumBits.h>

namespace Ludus::Engine::Core
{
	enum class ExecutionFlags : uint32_t
	{
		None = 0,
		PhysicsEnabled = 1 << 0,
		RenderingEnabled = 1 << 1,
		ScriptingEnabled = 1 << 2,
		SimulationEnabled = 1 << 3,
		SimulationSessionEnabled = 1 << 4
	};

	constexpr uint32_t DefaultExecutionMask =
		Ludus::Engine::Core::Enums::ToUnderlyingType(ExecutionFlags::None) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(ExecutionFlags::PhysicsEnabled) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(ExecutionFlags::RenderingEnabled) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(ExecutionFlags::ScriptingEnabled) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(ExecutionFlags::SimulationEnabled) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(ExecutionFlags::SimulationSessionEnabled);
}
