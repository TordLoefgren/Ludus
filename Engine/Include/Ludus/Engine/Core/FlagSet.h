#pragma once

#include <cstdint>

namespace Ludus::Engine::Core
{
	struct FlagSet
	{
		uint32_t Value = 0;

		constexpr FlagSet() = default;
		constexpr FlagSet(uint32_t value) : Value(value) {}

		void Set(uint32_t mask) { Value |= mask; }
		void Unset(uint32_t mask) { Value &= ~mask; }

		bool HasAny(uint32_t mask) const { return (Value & mask) != 0; }
		bool HasAll(uint32_t mask) const { return (Value & mask) == mask; }
	};
}
