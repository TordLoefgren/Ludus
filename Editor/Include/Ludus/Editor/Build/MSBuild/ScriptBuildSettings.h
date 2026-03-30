#pragma once

#include <filesystem>
#include <string>

namespace Ludus::Editor::Build::MSBuild
{
	struct ScriptBuildSettings
	{
		std::filesystem::path APIIncludeDirectory;
		std::filesystem::path OutDirectory;
		std::filesystem::path InDirectory;
		std::filesystem::path ScriptingProjectPath;
		std::string TargetName;
	};
}
