#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IBuildPipeline.h>
#include <Ludus/Editor/Build/IPackagePipeline.h>

namespace Ludus::Editor::Build
{
	struct BuildManager
	{
	private:
		std::unique_ptr<IBuildPipeline> m_BuildPipeline;
		std::unique_ptr<IPackagePipeline> m_PackagePipeline;

	public:
		explicit BuildManager(
			std::unique_ptr<IBuildPipeline> buildPipeline,
			std::unique_ptr<IPackagePipeline> packagePipeline
		);
		BuildManager();

		void Initialize();

		void BuildRuntime(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration
		);

		void CleanRuntime(
			const std::filesystem::path& projectRoot
		);

		void EnsureRuntimeHostProject(
			const std::filesystem::path& projectRoot
		);

		void EnsureScriptProject(
			const std::filesystem::path& projectRoot
		);

		void RunTargetBuildCommand(
			const std::filesystem::path& projectRoot,
			BuildTarget target,
			BuildCommand command,
			BuildConfiguration configuration
		);

		void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		);
	};
}
