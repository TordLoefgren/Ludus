#pragma once

#include <string_view>

namespace Ludus::Engine::Core::Build
{
	enum class OperatingSystem
	{
		Windows
	};

	inline constexpr std::string_view ToString(OperatingSystem operatingSystem)
	{
		switch (operatingSystem)
		{
			case OperatingSystem::Windows:
				return "Windows";
			default:
				return "Unknown";
		}
	}
}
