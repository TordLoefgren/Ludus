#pragma once

#include <cstdint>
#include <type_traits>

#include <Ludus/Engine/Core/Enums/EnumBits.h>

namespace Ludus::Engine::Core::Enums
{
	struct FlagSet
	{
		uint32_t Value = 0;

		constexpr FlagSet() = default;
		constexpr FlagSet(uint32_t value) : Value(value) { }

		void Set(uint32_t mask) { Value |= mask; }
		void Unset(uint32_t mask) { Value &= ~mask; }

		bool HasAny(uint32_t mask) const { return (Value & mask) != 0; }
		bool HasAll(uint32_t mask) const { return (Value & mask) == mask; }

		template<typename TEnum>
		void Set(TEnum flag) { Set(Ludus::Engine::Core::Enums::ToUnderlyingType(flag)); }

		template<typename TEnum>
		void Unset(TEnum flag) { Unset(Ludus::Engine::Core::Enums::ToUnderlyingType(flag)); }

		template<typename TEnum>
		bool HasAny(TEnum flag) const { return HasAny(Ludus::Engine::Core::Enums::ToUnderlyingType(flag)); }

		template<typename TEnum>
		bool HasAll(TEnum flag) const { return HasAll(Ludus::Engine::Core::Enums::ToUnderlyingType(flag)); }
	};
}
