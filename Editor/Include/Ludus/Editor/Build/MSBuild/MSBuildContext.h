#pragma once

#include <filesystem>
#include <optional>

namespace Ludus::Editor::Build::MSBuild
{
	struct MSBuildContext
	{
	private:
		std::optional<std::filesystem::path> LocateMSBuild();

	public:
		std::optional<std::filesystem::path> MSBuildPath;

		void Initialize();
	};
}
