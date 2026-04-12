#pragma once

#include <assert.h>
#include <concepts>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace Ludus::Engine::Core::Enums
{
	template<typename TEnum>
	inline bool constexpr always_false_t = false;

	template<typename TEnum>
	struct GetValues
	{
		static_assert(always_false_t<TEnum>, "GetValues<TEnum> is not specialized for this enum type.");
	};

	template<typename TEnum>
	concept HasValues = requires {
		GetValues<TEnum>::Values;
	};

	template<typename TEnum>
	concept HasToString = requires(TEnum value)
	{
		{ ToString(value) } -> std::convertible_to<std::string_view>;
	};

	template<typename TEnum>
	inline std::string GetDisplayName(TEnum value)
	{
		if constexpr (HasToString<TEnum>)
		{
			return std::string(ToString(value));
		}
		else
		{
			return std::to_string(static_cast<std::underlying_type_t<TEnum>>(value));
		}
	}

	template<typename TEnum>
		requires HasValues<TEnum>
	inline int IndexOf(TEnum value)
	{
		const auto& values = GetValues<TEnum>::Values;
		for (auto i = 0; i < static_cast<int>(values.size()); i++)
		{
			if (values[static_cast<std::size_t>(i)] == value)
			{
				return i;
			}
		}

		return -1;
	}

	template<typename TEnum>
		requires HasValues<TEnum>
	inline const std::vector<std::string>& GetDisplayNames()
	{
		// Immediately-invoked function expression (IIFE) to ensure one-time build.
		static const std::vector<std::string> names = []
		{
			const auto& values = GetValues<TEnum>::Values;

			std::vector<std::string> result;
			result.reserve(values.size());

			for (const auto& value : values)
			{
				result.emplace_back(GetDisplayName(value));
			}

			return result;
		}();

		return names;
	}
}
