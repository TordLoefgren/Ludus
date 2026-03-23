#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/IScriptBuildPipeline.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Engine/Core/Build/Configuration.h>

namespace Ludus::Editor::Build
{
	struct MSBuildScriptPipeline final : public IScriptBuildPipeline
	{
	private:
		std::optional<std::filesystem::path> m_MSBuildPath;

		std::optional<std::filesystem::path> LocateMSBuild();
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
		void Initialize();

		virtual void RunBuild(
			const std::filesystem::path& projectRoot,
			Ludus::Engine::Core::Build::Configuration configuration,
			BuildCommand command
		) override;

		virtual void EnsureScriptProject(
			const std::filesystem::path& projectRoot,
			const ScriptBuildSettings& settings
		) override;

		virtual void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		) override;
	};
}
