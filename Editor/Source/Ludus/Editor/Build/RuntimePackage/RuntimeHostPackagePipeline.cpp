#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Build/RuntimePackage/RuntimeHostPackagePipeline.h>
#include <Ludus/Editor/Persistence/BuildPaths.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace
{
	void StagePackagedRuntimeManifest(
		const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
		const std::filesystem::path& manifestFrom,
		const std::filesystem::path& outputRoot,
		std::string_view runtimeName
	)
	{
		auto runtimeManifest = runtimeManifestPersistence.Load(manifestFrom);

		Ludus::Engine::Persistence::Paths::ValidateRuntimeManifestPaths(runtimeManifest);

		runtimeManifestPersistence.Save(
			runtimeManifest,
			Ludus::Engine::Persistence::Paths::RuntimeManifestFile(outputRoot, runtimeName)
		);
	}

	void StagePackagedRuntimeLaunchSettings(
		const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence,
		const std::filesystem::path& settingsFrom,
		const std::filesystem::path& outputRoot,
		std::string_view runtimeName
	)
	{
		auto runtimeLaunchSettings = runtimeLaunchSettingsPersistence.Load(settingsFrom);

		runtimeLaunchSettingsPersistence.Save(
			runtimeLaunchSettings,
			Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(outputRoot, runtimeName)
		);
	}
}

namespace Ludus::Editor::Build::RuntimePackage
{
	RuntimeHostPackagePipeline::RuntimeHostPackagePipeline(
		const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
		const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence
	) :
		m_RuntimeManifestPersistence(runtimeManifestPersistence),
		m_RuntimeLaunchSettingsPersistence(runtimeLaunchSettingsPersistence)
	{}

	void RuntimeHostPackagePipeline::Initialize()
	{}

	void RuntimeHostPackagePipeline::BuildPackage(
		const std::filesystem::path& projectRoot,
		BuildConfiguration configuration,
		BuildPlatform platform
	)
	{
		const auto buildOutputDirectory = Ludus::Editor::Persistence::BuildPaths::RuntimeOutputDirectory(
			projectRoot,
			projectRoot.filename().string(),
			BuildOperatingSystem::Windows,
			platform,
			configuration
		);
		const auto runtimeHostBinDirectory = Ludus::Editor::Persistence::BuildPaths::RuntimeHostBinDirectory(
			projectRoot,
			platform,
			configuration
		);

		Ludus::Engine::FileSystem::ReplaceDirectory(
			Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot),
			Ludus::Engine::Persistence::Paths::AssetsDirectory(buildOutputDirectory)
		);
		Ludus::Engine::FileSystem::ReplaceDirectory(
			Ludus::Engine::Persistence::Paths::ResourcesDirectory(runtimeHostBinDirectory),
			Ludus::Engine::Persistence::Paths::ResourcesDirectory(buildOutputDirectory)
		);
		Ludus::Engine::FileSystem::ReplaceDirectory(
			Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot),
			Ludus::Engine::Persistence::Paths::ScenesDirectory(buildOutputDirectory)
		);
		StagePackagedRuntimeManifest(
			m_RuntimeManifestPersistence,
			Ludus::Engine::Persistence::Paths::RuntimeManifestFile(projectRoot, projectRoot.filename().string()),
			buildOutputDirectory,
			projectRoot.filename().string()
		);
		StagePackagedRuntimeLaunchSettings(
			m_RuntimeLaunchSettingsPersistence,
			Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(projectRoot, projectRoot.filename().string()),
			buildOutputDirectory,
			projectRoot.filename().string()
		);

		const auto scriptsModuleFrom = Ludus::Engine::Persistence::Paths::ScriptsDllFile(
			Ludus::Editor::Persistence::BuildPaths::ScriptsBinDirectory(
				projectRoot,
				platform,
				configuration
			)
		);
		const auto scriptsModuleTo = Ludus::Engine::Persistence::Paths::ScriptsDllFile(buildOutputDirectory);
		Ludus::Engine::FileSystem::CopyFileOverwrite(scriptsModuleFrom, scriptsModuleTo);

		for (const auto& directoryEntry : std::filesystem::recursive_directory_iterator(runtimeHostBinDirectory))
		{
			const auto& path = directoryEntry.path();

			if (path.extension() == Ludus::Editor::Persistence::BuildPaths::Constants::DynamicLibraryExtension)
			{
				Ludus::Engine::FileSystem::CopyFileOverwrite(path, buildOutputDirectory / path.filename());
			}
		}

		const auto executableFrom = Ludus::Editor::Persistence::BuildPaths::RuntimeHostExecutableFile(runtimeHostBinDirectory);
		const auto executableTo = Ludus::Editor::Persistence::BuildPaths::RuntimeExecutableFile(
			buildOutputDirectory,
			projectRoot.filename().string()
		);
		Ludus::Engine::FileSystem::CopyFileOverwrite(executableFrom, executableTo);
	}

	void RuntimeHostPackagePipeline::CleanPackage(
		const std::filesystem::path& projectRoot,
		BuildConfiguration configuration,
		BuildPlatform platform
	)
	{
		std::filesystem::remove_all(Ludus::Editor::Persistence::BuildPaths::BuildsDirectory(projectRoot));
		Ludus::Editor::Persistence::BuildPaths::EnsureRuntimeOutputDirectory(
			projectRoot,
			projectRoot.filename().string(),
			BuildOperatingSystem::Windows,
			platform,
			configuration
		);
	}
}
