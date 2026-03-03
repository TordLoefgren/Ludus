#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IScriptBuildPipeline.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/ProjectContext.h>

namespace Ludus::Editor::Build
{
	struct BuildManager
	{
	private:
		std::unique_ptr<IScriptBuildPipeline> m_Scripts;

		void RunScriptBuildCommand(
			const Ludus::Engine::Core::ProjectContext& context,
			Ludus::Engine::Core::Build::Configuration configuration,
			BuildCommand command
		);

	public:
		explicit BuildManager(std::unique_ptr<IScriptBuildPipeline> scriptBuildPipeline);
		BuildManager();

		void Initialize();

		void EnsureScriptProject(
			const Ludus::Engine::Core::ProjectContext& context,
			std::optional<ScriptBuildSettings> settings = std::nullopt
		);

		void CreateScript(
			Ludus::Engine::Core::ProjectContext& context,
			std::string_view name
		);

		void Build(
			const Ludus::Engine::Core::ProjectContext& context,
			Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug,
			BuildTarget target = BuildTarget::Scripts
		);

		void Clean(
			const Ludus::Engine::Core::ProjectContext& context,
			Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug,
			BuildTarget target = BuildTarget::Scripts
		);

		void Rebuild(
			const Ludus::Engine::Core::ProjectContext& context,
			Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug,
			BuildTarget target = BuildTarget::Scripts
		);
	};
}
