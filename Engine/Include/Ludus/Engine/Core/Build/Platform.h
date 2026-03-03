#pragma once

#include <string_view>

namespace Ludus::Engine::Core::Build
{
	enum class Platform
	{
		X64
	};

	inline constexpr std::string_view ToString(Platform platform)
	{
		switch (platform)
		{
			case Platform::X64:
				return "x64";
			default:
				return "x64";
		}
	}
}
