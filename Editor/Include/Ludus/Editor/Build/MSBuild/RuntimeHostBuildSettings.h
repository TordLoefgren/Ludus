#pragma once

#include <filesystem>
#include <string>

namespace Ludus::Editor::Build::MSBuild
{
	struct RuntimeHostBuildSettings
	{
		std::string TargetName;
		std::string RuntimeName;
		std::filesystem::path OutDirectory;
		std::filesystem::path InDirectory;
		std::filesystem::path EngineGladIncludeDirectory;
		std::filesystem::path EngineIncludeDirectory;
		std::filesystem::path EngineVendorIncludeDirectory;
		std::filesystem::path EngineResourcesIncludeDirectory;
		std::filesystem::path EngineProjectPath;
	};
}
