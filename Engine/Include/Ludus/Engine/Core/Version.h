#pragma once

#include <cstdint>
#include <format>
#include <string>

namespace Ludus::Engine::Core
{
	struct Version
	{
		uint32_t Major;
		uint32_t Minor;
		uint32_t Patch;

		static std::string ToString(Version version)
		{
			return std::format("v.{}.{}.{}", version.Major, version.Minor, version.Patch);
		}
	};
}
