#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Editor::Build
{
	enum class BuildPlatform
	{
		WindowsX64
	};

	inline constexpr std::string_view ToString(BuildPlatform platform)
	{
		switch (platform)
		{
			case BuildPlatform::WindowsX64:
				return "x64";
			default:
				throw std::runtime_error("Unexpected enum.");
		}
	}
}

template <>
struct Ludus::Engine::Core::Enums::GetValues<Ludus::Editor::Build::BuildPlatform>
{
	static constexpr std::array<Ludus::Editor::Build::BuildPlatform, 1> Values {
		Ludus::Editor::Build::BuildPlatform::WindowsX64
	};
};
