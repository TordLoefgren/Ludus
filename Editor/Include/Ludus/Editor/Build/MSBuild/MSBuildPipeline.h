#pragma once

#include <filesystem>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IBuildPipeline.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildContext.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildRuntimeHostPipeline.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildScriptPipeline.h>

namespace Ludus::Engine::Persistence
{
	class IRuntimeManifestPersistence;
}

namespace Ludus::Editor::Build::MSBuild
{
	struct MSBuildPipeline final : public IBuildPipeline
	{
	private:
		MSBuildContext m_Context;
		MSBuildRuntimeHostPipeline m_RuntimeHostPipeline;
		MSBuildScriptPipeline m_ScriptPipeline;

	public:
		explicit MSBuildPipeline(const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence);

		virtual void Initialize() override;

		virtual void RunBuild(
			const std::filesystem::path& projectRoot,
			BuildTarget target,
			BuildConfiguration configuration,
			BuildCommand command
		) override;

		virtual void EnsureProject(
			const std::filesystem::path& projectRoot,
			BuildTarget target
		) override;

		virtual void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		) override;
	};
}
