#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildContext.h>
#include <Ludus/Editor/Build/MSBuild/ScriptBuildSettings.h>

namespace Ludus::Editor::Build::MSBuild
{
	struct MSBuildScriptPipeline
	{
	private:
		MSBuildContext& m_Context;

		void CopyTemplateToDestinationIfMissing(
			const std::filesystem::path& templateRoot,
			std::string_view templateFileName,
			const std::filesystem::path& destinationPath
		) const;

		void AddScript(std::string_view name, const std::filesystem::path& projectRoot);
		void AddScriptsModuleReference(std::string_view name, const std::filesystem::path& projectRoot);
		void AddScriptsProjectReference(std::string_view name, const std::filesystem::path& projectRoot);
		void EnsureBuildFiles(const std::filesystem::path& projectRoot);

		void SetScriptProjectCompilationSettings(
			const std::filesystem::path& projectRoot,
			const ScriptBuildSettings& settings
		);

	public:
		MSBuildScriptPipeline(MSBuildContext& context);

		void RunBuild(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildCommand command
		);

		void EnsureScriptProject(
			const std::filesystem::path& projectRoot,
			const ScriptBuildSettings& settings
		);

		void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		);
	};
}
