#pragma once

#include <filesystem>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Engine/Core/Build/Configuration.h>

namespace Ludus::Editor::Build
{
	struct IScriptBuildPipeline
	{
		virtual ~IScriptBuildPipeline() = default;

		virtual void Initialize() = 0;

		virtual void RunBuild(
			const std::filesystem::path& projectRoot,
			Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug,
			BuildCommand command = BuildCommand::Build
		) = 0;

		virtual void EnsureScriptProject(const std::filesystem::path& projectRoot, const ScriptBuildSettings& settings) = 0;
		virtual void CreateScript(const std::filesystem::path& projectRoot, std::string_view name) = 0;
	};
}
