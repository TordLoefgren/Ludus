#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Editor::Build
{
	enum class BuildConfiguration
	{
		Debug,
		Release
	};

	inline constexpr std::string_view ToString(BuildConfiguration configuration)
	{
		switch (configuration)
		{
			case BuildConfiguration::Debug:
				return "Debug";
			case BuildConfiguration::Release:
				return "Release";
			default:
				throw std::runtime_error("Unsupported build configuration.");
		}
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Editor::Build::BuildConfiguration>
{
	static constexpr std::array<Ludus::Editor::Build::BuildConfiguration, 2> Values {
		Ludus::Editor::Build::BuildConfiguration::Debug,
		Ludus::Editor::Build::BuildConfiguration::Release
	};
};
