#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildContext.h>
#include <Ludus/Editor/Build/MSBuild/RuntimeHostBuildSettings.h>

namespace Ludus::Editor::Build::MSBuild
{
	struct MSBuildRuntimeHostPipeline
	{
	private:
		MSBuildContext& m_Context;

		void CopyTemplateToDestinationIfMissing(
			const std::filesystem::path& templateRoot,
			std::string_view templateFileName,
			const std::filesystem::path& destinationPath
		) const;

		void EnsureBuildFiles(const std::filesystem::path& projectRoot);
		void PopulateRuntimeHostOutput(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration
		);

		void SetRuntimeHostProjectCompilationSettings(
			const std::filesystem::path& projectRoot,
			const RuntimeHostBuildSettings& settings
		);
		void SetRuntimeHostModuleManifestName(
			const std::filesystem::path& projectRoot,
			const RuntimeHostBuildSettings& settings
		);

	public:
		MSBuildRuntimeHostPipeline(MSBuildContext& context);

		void RunBuild(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildCommand command
		);

		void EnsureRuntimeHostProject(
			const std::filesystem::path& projectRoot,
			const RuntimeHostBuildSettings& settings
		);
	};
}
