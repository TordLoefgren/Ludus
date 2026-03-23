#pragma once

#include <filesystem>

#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/Build/Platform.h>

namespace Ludus::Engine::Runtime
{
	struct RuntimeEnvironment
	{
		std::filesystem::path RuntimeRootDirectory;
		Ludus::Engine::Core::Build::Platform ScriptModulePlatform = Ludus::Engine::Core::Build::Platform::WindowsX64;
		Ludus::Engine::Core::Build::Configuration ScriptModuleConfiguration = Ludus::Engine::Core::Build::Configuration::Debug;
	};
}
