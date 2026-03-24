#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IScriptBuildPipeline.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>

namespace Ludus::Editor::Build
{
	struct BuildManager
	{
	private:
		std::unique_ptr<IScriptBuildPipeline> m_Scripts;

		void RunScriptBuildCommand(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildCommand command
		);

	public:
		explicit BuildManager(std::unique_ptr<IScriptBuildPipeline> scriptBuildPipeline);
		BuildManager();

		void Initialize();

		void EnsureScriptProject(
			const std::filesystem::path& projectRoot,
			std::optional<ScriptBuildSettings> settings = std::nullopt
		);

		void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		);

		void Build(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration = BuildConfiguration::Debug,
			BuildTarget target = BuildTarget::Scripts
		);

		void Clean(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration = BuildConfiguration::Debug,
			BuildTarget target = BuildTarget::Scripts
		);

		void Rebuild(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration = BuildConfiguration::Debug,
			BuildTarget target = BuildTarget::Scripts
		);
	};
}
