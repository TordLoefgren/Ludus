#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Enums/EnumTraits.h>

namespace Ludus::Editor::Build
{
	enum class BuildOperatingSystem
	{
		Windows
	};

	inline constexpr std::string_view ToString(BuildOperatingSystem operatingSystem)
	{
		switch (operatingSystem)
		{
			case BuildOperatingSystem::Windows:
				return "Windows";
			default:
				throw std::runtime_error("Unsupported build operating system.");
		}
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Editor::Build::BuildOperatingSystem>
{
	static constexpr std::array<Ludus::Editor::Build::BuildOperatingSystem, 1> Values {
		Ludus::Editor::Build::BuildOperatingSystem::Windows
	};
};
