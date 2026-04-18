#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>

namespace Ludus::Editor::Build
{
	struct IBuildPipeline;
	struct IPackagePipeline;
}

namespace Ludus::Engine::Persistence
{
	class IRuntimeLaunchSettingsPersistence;
	class IRuntimeManifestPersistence;
}

namespace Ludus::Editor::Build
{
	struct BuildManager
	{
	private:
		std::unique_ptr<IBuildPipeline> m_BuildPipeline;
		std::unique_ptr<IPackagePipeline> m_PackagePipeline;

		BuildManager(
			std::unique_ptr<IBuildPipeline> buildPipeline,
			std::unique_ptr<IPackagePipeline> packagePipeline
		);

	public:
		static BuildManager Create(
			const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
			const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence
		);
		~BuildManager();

		BuildManager(const BuildManager&) = delete;
		BuildManager& operator=(const BuildManager&) = delete;
		BuildManager(BuildManager&&) noexcept;
		BuildManager& operator=(BuildManager&&) noexcept;

		void Initialize();

		void BuildRuntime(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration
		);

		void CleanRuntime(
			const std::filesystem::path& projectRoot
		);

		void EnsureRuntimeHostProject(
			const std::filesystem::path& projectRoot
		);

		void EnsureScriptProject(
			const std::filesystem::path& projectRoot
		);

		void RunTargetBuildCommand(
			const std::filesystem::path& projectRoot,
			BuildTarget target,
			BuildCommand command,
			BuildConfiguration configuration
		);

		void CreateScript(
			const std::filesystem::path& projectRoot,
			std::string_view name
		);
	};
}
