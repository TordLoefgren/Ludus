#pragma once

#include <filesystem>
#include <string>

namespace Ludus::Editor::Build
{
	struct ScriptBuildSettings
	{
		std::filesystem::path IncludeDirectory;
		std::filesystem::path OutDirectory;
		std::filesystem::path InDirectory;
		std::string TargetName;
	};
}
