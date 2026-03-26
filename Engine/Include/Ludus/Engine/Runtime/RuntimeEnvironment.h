#pragma once

#include <filesystem>

namespace Ludus::Engine::Runtime
{
	struct RuntimeEnvironment
	{
		std::filesystem::path RuntimeRootDirectory;
		std::filesystem::path ResourcesDirectory;
		std::filesystem::path ShadersDirectory;
		std::filesystem::path DefaultFontPath;
		std::filesystem::path RuntimeManifestPath;
		std::filesystem::path ScriptModulePath;
	};
}
