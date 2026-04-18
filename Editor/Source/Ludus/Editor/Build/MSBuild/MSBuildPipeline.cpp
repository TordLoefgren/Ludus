#include "pch.h"

#include <filesystem>
#include <stdexcept>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildPipeline.h>
#include <Ludus/Editor/Build/MSBuild/RuntimeHostBuildSettings.h>
#include <Ludus/Editor/Build/MSBuild/ScriptBuildSettings.h>
#include <Ludus/Editor/Persistence/BuildPaths.h>
#include <Ludus/Editor/Persistence/RepositoryPaths.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>

namespace
{
	struct Constants
	{
		static constexpr std::string_view GladDirectory = "glad";
		static constexpr std::string_view GladIncludeDirectory = "include";
		static constexpr std::string_view EngineProjectFile = "Engine.vcxproj";
		static constexpr std::string_view ScriptingProjectFile = "Scripting.vcxproj";
	};
}

namespace Ludus::Editor::Build::MSBuild
{
	MSBuildPipeline::MSBuildPipeline(
		const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence
	) :
		m_Context(),
		m_RuntimeHostPipeline(m_Context, runtimeManifestPersistence),
		m_ScriptPipeline(m_Context)
	{}

	void MSBuildPipeline::Initialize()
	{
		m_Context.Initialize();
	}

	void MSBuildPipeline::RunBuild(const std::filesystem::path& projectRoot, BuildTarget target, BuildConfiguration configuration, BuildCommand command)
	{
		switch (target)
		{
			case Ludus::Editor::Build::BuildTarget::RuntimeHost:
				m_RuntimeHostPipeline.RunBuild(projectRoot, configuration, command);
				return;

			case Ludus::Editor::Build::BuildTarget::Scripts:
				m_ScriptPipeline.RunBuild(projectRoot, configuration, command);
				return;

			default:
				throw std::runtime_error("Unsupported build target.");
		}
	}

	void MSBuildPipeline::EnsureProject(const std::filesystem::path& projectRoot, BuildTarget target)
	{
		switch (target)
		{
			case Ludus::Editor::Build::BuildTarget::RuntimeHost:
			{
				RuntimeHostBuildSettings settings = {
					std::string(Ludus::Editor::Persistence::BuildPaths::Constants::RuntimeHostTarget),
					projectRoot.filename().string(),
					Ludus::Editor::Persistence::BuildPaths::RuntimeHostBinDirectory(projectRoot),
					Ludus::Editor::Persistence::BuildPaths::RuntimeHostObjDirectory(projectRoot),
					Ludus::Editor::Persistence::RepositoryPaths::EngineVendorsDirectory() / std::string(Constants::GladDirectory) / std::string(Constants::GladIncludeDirectory),
					Ludus::Editor::Persistence::RepositoryPaths::EngineIncludeDirectory(),
					Ludus::Editor::Persistence::RepositoryPaths::EngineVendorsDirectory(),
					Ludus::Editor::Persistence::RepositoryPaths::EngineResourcesDirectory(),
					Ludus::Editor::Persistence::RepositoryPaths::EngineDirectory() / std::string(Constants::EngineProjectFile),
				};

				m_RuntimeHostPipeline.EnsureRuntimeHostProject(projectRoot, settings);
				return;
			}

			case Ludus::Editor::Build::BuildTarget::Scripts:
			{
				ScriptBuildSettings settings = {
					Ludus::Editor::Persistence::RepositoryPaths::ScriptingAPIIncludeDirectory(),
					Ludus::Editor::Persistence::BuildPaths::ScriptsBinDirectory(projectRoot),
					Ludus::Editor::Persistence::BuildPaths::ScriptsObjDirectory(projectRoot),
					Ludus::Editor::Persistence::RepositoryPaths::ScriptingDirectory() / std::string(Constants::ScriptingProjectFile),
					std::string(Ludus::Editor::Persistence::BuildPaths::Constants::ScriptsTarget)
				};

				m_ScriptPipeline.EnsureScriptProject(projectRoot, settings);
				return;
			}

			default:
				throw std::runtime_error("Unsupported build target.");
		}
	}

	void MSBuildPipeline::CreateScript(const std::filesystem::path& projectRoot, std::string_view name)
	{
		m_ScriptPipeline.CreateScript(projectRoot, name);
	}
}
