#pragma once

#include <cstdint>
#include <type_traits>

namespace Ludus::Engine::Core
{
	template <typename TEnum>
	constexpr std::uint32_t Mask(TEnum e)
	{
		static_assert(std::is_enum_v<TEnum>, "Mask must be an enum.");
		return static_cast<std::uint32_t>(e);
	}
}
