#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/IScriptBuildPipeline.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/ProjectContext.h>
#include <Ludus/Engine/Core/Random.h>

namespace Ludus::Editor::Build
{
	struct MSBuildScriptPipeline final : public IScriptBuildPipeline
	{
	private:
		Ludus::Engine::Core::Random m_Random { };
		std::optional<std::filesystem::path> m_MSBuildPath;

		std::optional<std::filesystem::path> LocateMSBuild();
		void CopyTemplateToDestinationIfMissing(
			const std::filesystem::path& templateRoot,
			std::string_view templateFileName,
			const std::filesystem::path& destinationPath
		) const;

		void AddScript(std::string_view name, const Ludus::Engine::Core::ProjectContext& context);
		void AddScriptsModuleReference(std::string_view name, const Ludus::Engine::Core::ProjectContext& context);
		void AddScriptsProjectReference(std::string_view name, const Ludus::Engine::Core::ProjectContext& context);
		void EnsureBuildFiles(const Ludus::Engine::Core::ProjectContext& context);

		void SetScriptProjectCompilationSettings(
			const Ludus::Engine::Core::ProjectContext& context,
			const ScriptBuildSettings& settings
		);

	public:
		void Initialize();

		virtual void RunBuild(
			const Ludus::Engine::Core::ProjectContext& context,
			Ludus::Engine::Core::Build::Configuration configuration,
			BuildCommand command
		) override;

		virtual void EnsureScriptProject(
			const Ludus::Engine::Core::ProjectContext& context,
			const ScriptBuildSettings& settings
		) override;

		virtual void CreateScript(
			Ludus::Engine::Core::ProjectContext& context,
			std::string_view name
		) override;
	};
}
