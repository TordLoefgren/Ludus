#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace Ludus::Engine::Core::Enums
{
	template<typename T>
	struct EnableBitMaskOperators : std::false_type { };

	template<typename T>
	concept BitMaskEnum = std::is_enum_v<T> && EnableBitMaskOperators<T>::value;

	template <typename T>
	constexpr auto ToUnderlyingType(T value) -> std::underlying_type_t<T>
	{
		static_assert(std::is_enum_v<T>, "ToUnderlyingType requires an enum type.");
		return static_cast<std::underlying_type_t<T>>(value);
	}
}

namespace Ludus
{
	template<Engine::Core::Enums::BitMaskEnum T>
	constexpr T operator|(T a, T b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
	}

	template<Engine::Core::Enums::BitMaskEnum T>
	constexpr T& operator|=(T& a, T b)
	{
		a = a | b;
		return a;
	}

	template<Engine::Core::Enums::BitMaskEnum T>
	constexpr T operator&(T a, T b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<T>(static_cast<U>(a) & static_cast<U>(b));
	}

	template<Engine::Core::Enums::BitMaskEnum T>
	constexpr T& operator&=(T& a, T b)
	{
		a = a & b;
		return a;
	}
}
