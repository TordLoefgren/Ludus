#pragma once

#include <string_view>

namespace Ludus::Engine::Runtime
{
	struct RuntimeLauncher
	{
		static int Run(std::string_view runtimeName);
	};
}
