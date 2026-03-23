#pragma once

#include <string_view>

namespace Ludus::Engine::Core::Build
{
	// In order to avoid macro redefitions for X64 we prepend "Windows" to it.
	enum class Platform
	{
		WindowsX64
	};

	inline constexpr std::string_view ToString(Platform platform)
	{
		switch (platform)
		{
			case Platform::WindowsX64:
				return "x64";
			default:
				return "Unknown";
		}
	}
}
