#pragma once

#include <filesystem>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>

namespace Ludus::Editor::Build
{
	struct IBuildPipeline
	{
		virtual ~IBuildPipeline() = default;

		virtual void Initialize() = 0;

		virtual void RunBuild(
			const std::filesystem::path& projectRoot,
			BuildTarget target,
			BuildConfiguration configuration,
			BuildCommand command
		) = 0;

		virtual void EnsureProject(
			const std::filesystem::path& projectRoot,
			BuildTarget target
		) = 0;

		virtual void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		) = 0;
	};
}
