#pragma once

#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/Build/Platform.h>

namespace Ludus::Engine::Core
{
	struct ScriptRuntimeState
	{
		Ludus::Engine::Core::Build::Platform Platform = Ludus::Engine::Core::Build::Platform::X64;
		Ludus::Engine::Core::Build::Configuration Configuration = Ludus::Engine::Core::Build::Configuration::Debug;
	};
}
