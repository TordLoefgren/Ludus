#include "pch.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IScriptBuildPipeline.h>
#include <Ludus/Editor/Build/MSBuildScriptPipeline.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Editor/Persistence/Paths.h>

namespace Ludus::Editor::Build
{
	BuildManager::BuildManager(
		std::unique_ptr<IScriptBuildPipeline> scriptBuildPipeline
	)
	{
		m_Scripts = std::move(scriptBuildPipeline);
	}

	BuildManager::BuildManager()
		: BuildManager(std::make_unique<MSBuildScriptPipeline>())
	{ }

	void BuildManager::Initialize()
	{
		if (!m_Scripts)
		{
			throw std::runtime_error("The script build pipeline has not been initialized.");
		}

		m_Scripts->Initialize();
	}

	void BuildManager::EnsureScriptProject(
		const std::filesystem::path& projectRoot,
		std::optional<ScriptBuildSettings> settings
	)
	{
		if (!settings.has_value())
		{
			settings = {
				Ludus::Editor::Persistence::Paths::EngineScriptingApiScriptsIncludeDir(),
				Ludus::Editor::Persistence::Paths::ScriptsBinDirectory(projectRoot),
				Ludus::Editor::Persistence::Paths::ScriptsObjDirectory(projectRoot),
				std::string(Ludus::Editor::Persistence::Paths::Constants::ScriptTargetName)
			};
		}

		m_Scripts->EnsureScriptProject(projectRoot, settings.value());
	}

	void BuildManager::RunScriptBuildCommand(
		const std::filesystem::path& projectRoot,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildCommand command
	)
	{
		EnsureScriptProject(projectRoot);
		m_Scripts->RunBuild(projectRoot, configuration, command);
	}

	void BuildManager::CreateScript(const std::filesystem::path& projectRoot, std::string_view name)
	{
		EnsureScriptProject(projectRoot);
		m_Scripts->CreateScript(projectRoot, name);
	}

	void BuildManager::Build(
		const std::filesystem::path& projectRoot,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildTarget target
	)
	{
		if (target == BuildTarget::Scripts)
		{
			RunScriptBuildCommand(projectRoot, configuration, BuildCommand::Build);
		}
		else
		{
			throw std::runtime_error("Unknown build target.");
		}
	}

	void BuildManager::Clean(
		const std::filesystem::path& projectRoot,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildTarget target
	)
	{
		switch (target)
		{
			case BuildTarget::Scripts:
				RunScriptBuildCommand(projectRoot, configuration, BuildCommand::Clean);
				break;
			default:
				throw std::runtime_error("Unknown build target.");
		}
	}

	void BuildManager::Rebuild(
		const std::filesystem::path& projectRoot,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildTarget target
	)
	{
		if (target == BuildTarget::Scripts)
		{
			RunScriptBuildCommand(projectRoot, configuration, BuildCommand::Rebuild);
		}
		else
		{
			throw std::runtime_error("Unknown build target.");
		}
	}
}
