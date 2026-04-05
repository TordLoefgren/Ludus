#pragma once

#include <array>
#include <compare>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace Ludus::Engine::Platform
{
	struct Guid
	{
		std::array<std::uint8_t, 16> Bytes { };

		bool IsEmpty() const;

		// Format: lowercase 8-4-4-4-12.
		std::string ToString() const;

		// Use std::array default comparison.
		auto operator<=>(const Guid&) const = default;
	};

	Guid CreateGuid();

	std::optional<Guid> TryParse(std::string_view text);
}

template<>
struct std::hash<Ludus::Engine::Platform::Guid>
{
	std::size_t operator()(const Ludus::Engine::Platform::Guid& guid) const noexcept
	{
		std::size_t hash = 0;
		for (auto byte : guid.Bytes)
		{
			hash = hash * 37 + byte;
		}
		return hash;
	}
};
