#pragma once

#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/ScriptBuildSettings.h>
#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/ProjectContext.h>

namespace Ludus::Editor::Build
{
	struct IScriptBuildPipeline
	{
		virtual ~IScriptBuildPipeline() = default;

		virtual void Initialize() = 0;

		virtual void RunBuild(
			const Ludus::Engine::Core::ProjectContext& context,
			Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug,
			BuildCommand command = BuildCommand::Build
		) = 0;

		virtual void EnsureScriptProject(const Ludus::Engine::Core::ProjectContext& context, const ScriptBuildSettings& settings) = 0;
		virtual void CreateScript(Ludus::Engine::Core::ProjectContext& context, std::string_view name) = 0;
	};
}
