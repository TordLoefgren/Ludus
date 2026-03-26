#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildOperatingSystem.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Persistence/Paths.h>

namespace Ludus::Editor::Persistence::BuildPaths
{
	namespace Constants
	{
		inline constexpr std::string_view BinDirectory = "bin";
		inline constexpr std::string_view ObjDirectory = "obj";
		inline constexpr std::string_view BuildsDirectory = "builds";
		inline constexpr std::string_view ScriptsDirectory = "Scripts";
		inline constexpr std::string_view ScriptsTarget = "Scripts";
		inline constexpr std::string_view RuntimeHostDirectory = "RuntimeHost";
		inline constexpr std::string_view RuntimeHostExecutableFile = "RuntimeHost.exe";
		inline constexpr std::string_view RuntimeHostModuleSourceFile = "RuntimeHostModule.cpp";
		inline constexpr std::string_view RuntimeHostProjectFile = "RuntimeHost.vcxproj";
		inline constexpr std::string_view RuntimeHostTarget = "RuntimeHost";
		inline constexpr std::string_view DynamicLibraryExtension = ".dll";
		inline constexpr std::string_view ExecutableExtension = ".exe";
	}

	inline std::filesystem::path BinDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / std::string(Constants::BinDirectory);
	}

	inline std::filesystem::path ObjDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / std::string(Constants::ObjDirectory);
	}

	inline std::filesystem::path BuildsDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / std::string(Constants::BuildsDirectory);
	}

	inline std::filesystem::path RuntimeOutputDirectory(
		const std::filesystem::path& projectRoot,
		std::string_view runtimeName,
		const Ludus::Editor::Build::BuildOperatingSystem operatingSystem = Ludus::Editor::Build::BuildOperatingSystem::Windows,
		const Ludus::Editor::Build::BuildPlatform platform = Ludus::Editor::Build::BuildPlatform::WindowsX64,
		const Ludus::Editor::Build::BuildConfiguration configuration = Ludus::Editor::Build::BuildConfiguration::Debug
	)
	{
		return BuildsDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(operatingSystem)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration)) /
			std::string(runtimeName);
	}

	inline void EnsureRuntimeOutputDirectory(
		const std::filesystem::path& projectRoot,
		std::string_view runtimeName,
		const Ludus::Editor::Build::BuildOperatingSystem operatingSystem = Ludus::Editor::Build::BuildOperatingSystem::Windows,
		const Ludus::Editor::Build::BuildPlatform platform = Ludus::Editor::Build::BuildPlatform::WindowsX64,
		const Ludus::Editor::Build::BuildConfiguration configuration = Ludus::Editor::Build::BuildConfiguration::Debug
	)
	{
		const auto runtimeOutputDirectory = RuntimeOutputDirectory(projectRoot, runtimeName, operatingSystem, platform, configuration);
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::AssetsDirectory(runtimeOutputDirectory));
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::ResourcesDirectory(runtimeOutputDirectory));
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::ScenesDirectory(runtimeOutputDirectory));
	}

	inline std::filesystem::path ScriptsBinDirectory(const std::filesystem::path& projectRoot)
	{
		return BinDirectory(projectRoot) / std::string(Constants::ScriptsDirectory);
	}

	inline std::filesystem::path ScriptsBinDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildPlatform platform,
		const Ludus::Editor::Build::BuildConfiguration configuration
	)
	{
		return ScriptsBinDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path ScriptsObjDirectory(const std::filesystem::path& projectRoot)
	{
		return ObjDirectory(projectRoot) / std::string(Constants::ScriptsDirectory);
	}

	inline std::filesystem::path ScriptsObjDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildPlatform platform,
		const Ludus::Editor::Build::BuildConfiguration configuration
	)
	{
		return ScriptsObjDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path ScriptsDllFile(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildPlatform platform = Ludus::Editor::Build::BuildPlatform::WindowsX64,
		const Ludus::Editor::Build::BuildConfiguration configuration = Ludus::Editor::Build::BuildConfiguration::Debug
	)
	{
		return ScriptsBinDirectory(projectRoot, platform, configuration) /
			std::string(Ludus::Engine::Persistence::Paths::Constants::ScriptsModuleFile);
	}

	inline void EnsureProjectScriptsBuildLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(ScriptsBinDirectory(projectRoot));
		std::filesystem::create_directories(ScriptsObjDirectory(projectRoot));
	}

	inline std::filesystem::path RuntimeHostBinDirectory(const std::filesystem::path& projectRoot)
	{
		return BinDirectory(projectRoot) / std::string(Constants::RuntimeHostDirectory);
	}

	inline std::filesystem::path RuntimeHostBinDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildPlatform platform,
		const Ludus::Editor::Build::BuildConfiguration configuration
	)
	{
		return RuntimeHostBinDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path RuntimeHostExecutableFile(const std::filesystem::path& runtimeHostRoot)
	{
		return runtimeHostRoot / std::string(Constants::RuntimeHostExecutableFile);
	}

	inline std::filesystem::path RuntimeExecutableFile(const std::filesystem::path& runtimeOutputRoot, std::string_view runtimeName)
	{
		return runtimeOutputRoot / (std::string(runtimeName) + std::string(Constants::ExecutableExtension));
	}

	inline std::filesystem::path RuntimeHostObjDirectory(const std::filesystem::path& projectRoot)
	{
		return ObjDirectory(projectRoot) / std::string(Constants::RuntimeHostDirectory);
	}

	inline std::filesystem::path RuntimeHostObjDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildPlatform platform,
		const Ludus::Editor::Build::BuildConfiguration configuration
	)
	{
		return RuntimeHostObjDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path RuntimeHostModuleFile(const std::filesystem::path& projectRoot)
	{
		return RuntimeHostObjDirectory(projectRoot) / std::string(Constants::RuntimeHostModuleSourceFile);
	}

	inline std::filesystem::path RuntimeHostProjectFile(const std::filesystem::path& projectRoot)
	{
		return RuntimeHostObjDirectory(projectRoot) / std::string(Constants::RuntimeHostProjectFile);
	}

	inline void EnsureProjectRuntimeHostBuildLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(RuntimeHostBinDirectory(projectRoot));
		std::filesystem::create_directories(RuntimeHostObjDirectory(projectRoot));
	}

	inline void EnsureProjectRuntimeHostLayoutExists(const std::filesystem::path& projectRoot)
	{
		EnsureProjectRuntimeHostBuildLayoutExists(projectRoot);
	}
}
