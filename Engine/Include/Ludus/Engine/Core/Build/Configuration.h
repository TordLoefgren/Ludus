#pragma once

#include <string_view>

namespace Ludus::Engine::Core::Build
{
	enum class Configuration
	{
		Debug,
		Release
	};

	inline constexpr std::string_view ToString(Configuration configuration)
	{
		switch (configuration)
		{
			case Configuration::Debug:
				return "Debug";
			case Configuration::Release:
				return "Release";
			default:
				return "Debug";
		}
	}
}
