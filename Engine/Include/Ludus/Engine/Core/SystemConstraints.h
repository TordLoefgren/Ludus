#pragma once

#include <cstdint>

namespace Ludus::Engine::Core
{
	struct SystemConstraints
	{
		uint32_t ForbidAny = 0;
		uint32_t RequireAll = 0;
		uint32_t RequireAny = 0;

		constexpr SystemConstraints() = default;
		constexpr SystemConstraints(uint32_t forbidAny, uint32_t requreAll, uint32_t requireAny)
			: ForbidAny(forbidAny), RequireAll(requreAll), RequireAny(requireAny)
		{ }

		static SystemConstraints Create() { return SystemConstraints { }; }

		constexpr SystemConstraints& Forbid(uint32_t mask) { ForbidAny |= mask; return *this; }
		constexpr SystemConstraints& RequireAllOf(uint32_t mask) { RequireAll |= mask; return *this; }
		constexpr SystemConstraints& RequireAnyOf(uint32_t mask) { RequireAny |= mask; return *this; }
	};
}
