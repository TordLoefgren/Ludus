#include "pch.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IScriptBuildPipeline.h>
#include <Ludus/Editor/Build/MSBuildScriptPipeline.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Core/ProjectContext.h>

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
		const Ludus::Engine::Core::ProjectContext& context,
		std::optional<ScriptBuildSettings> settings
	)
	{
		if (!settings.has_value())
		{
			settings = {
				Ludus::Editor::Persistence::Paths::EngineScriptingApiScriptsIncludeDir(),
				Ludus::Editor::Persistence::Paths::ScriptsBinDirectory(context.ProjectRootDirectory),
				Ludus::Editor::Persistence::Paths::ScriptsObjDirectory(context.ProjectRootDirectory),
				std::string(Ludus::Editor::Persistence::Paths::Constants::ScriptTargetName)
			};
		}

		m_Scripts->EnsureScriptProject(context, settings.value());
	}

	void BuildManager::RunScriptBuildCommand(
		const Ludus::Engine::Core::ProjectContext& context,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildCommand command
	)
	{
		EnsureScriptProject(context);
		m_Scripts->RunBuild(context, configuration, command);
	}

	void BuildManager::CreateScript(Ludus::Engine::Core::ProjectContext& context, std::string_view name)
	{
		EnsureScriptProject(context);
		m_Scripts->CreateScript(context, name);
	}

	void BuildManager::Build(
		const Ludus::Engine::Core::ProjectContext& context,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildTarget target
	)
	{
		if (target == BuildTarget::Scripts)
		{
			RunScriptBuildCommand(context, configuration, BuildCommand::Build);
		}
		else
		{
			throw std::runtime_error("Unknown build target.");
		}
	}

	void BuildManager::Clean(
		const Ludus::Engine::Core::ProjectContext& context,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildTarget target
	)
	{
		switch (target)
		{
			case BuildTarget::Scripts:
				RunScriptBuildCommand(context, configuration, BuildCommand::Clean);
				break;
			default:
				throw std::runtime_error("Unknown build target.");
		}
	}

	void BuildManager::Rebuild(
		const Ludus::Engine::Core::ProjectContext& context,
		Ludus::Engine::Core::Build::Configuration configuration,
		BuildTarget target
	)
	{
		if (target == BuildTarget::Scripts)
		{
			RunScriptBuildCommand(context, configuration, BuildCommand::Rebuild);
		}
		else
		{
			throw std::runtime_error("Unknown build target.");
		}
	}
}
