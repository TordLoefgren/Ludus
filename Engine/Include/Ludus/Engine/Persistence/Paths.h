#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/Build/OperatingSystem.h>
#include <Ludus/Engine/Core/Build/Platform.h>
#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Engine::Persistence::Paths
{
	namespace Constants
	{
		inline constexpr std::string_view AssetsDirectoryName = "Assets";
		inline constexpr std::string_view BuildsDirectoryName = "Builds";
		inline constexpr std::string_view ScenesDirectoryName = "Scenes";
		inline constexpr std::string_view ScriptsDirectoryName = "Scripts";
		inline constexpr std::string_view BinDirectoryName = "Bin";
		inline constexpr std::string_view ScriptsDllFileName = "Scripts.dll";
		inline constexpr std::string_view RuntimeFileExtension = ".ludus.runtime";
		inline constexpr std::string_view SceneFileExtension = ".ludus.scene";
	}

	inline std::filesystem::path AssetsDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::AssetsDirectoryName);
	}

	inline std::filesystem::path BuildsDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::BuildsDirectoryName);
	}

	inline std::filesystem::path BuildOutputDirectory(
		const std::filesystem::path& runtimeRootDirectory,
		const Ludus::Engine::Core::Build::OperatingSystem operatingSystem = Ludus::Engine::Core::Build::OperatingSystem::Windows,
		const Ludus::Engine::Core::Build::Platform platform = Ludus::Engine::Core::Build::Platform::WindowsX64,
		const Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug
	)
	{
		return BuildsDirectory(runtimeRootDirectory) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(operatingSystem)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path ScenesDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::ScenesDirectoryName);
	}

	inline std::filesystem::path SceneFile(const std::filesystem::path& runtimeRootDirectory, std::string_view sceneName)
	{
		return ScenesDirectory(runtimeRootDirectory) / (std::string(sceneName) + std::string(Constants::SceneFileExtension));
	}

	inline std::filesystem::path RuntimeManifestFile(std::string_view runtimeName)
	{
		return std::string(runtimeName) + std::string(Constants::RuntimeFileExtension);
	}

	inline std::filesystem::path RuntimeManifestFile(const std::filesystem::path& runtimeRootDirectory, std::string_view runtimeName)
	{
		return runtimeRootDirectory / RuntimeManifestFile(runtimeName);
	}

	inline std::filesystem::path BinDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::BinDirectoryName);
	}

	inline std::filesystem::path ScriptsBinDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return BinDirectory(runtimeRootDirectory) / std::string(Constants::ScriptsDirectoryName);
	}

	inline std::filesystem::path ScriptsBinDirectory(
		const std::filesystem::path& runtimeRootDirectory,
		const Ludus::Engine::Core::Build::Platform platform,
		const Ludus::Engine::Core::Build::Configuration configuration
	)
	{
		return ScriptsBinDirectory(runtimeRootDirectory) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path ScriptsDllFile(
		const std::filesystem::path& runtimeRootDirectory,
		const Ludus::Engine::Core::Build::Platform platform = Ludus::Engine::Core::Build::Platform::WindowsX64,
		const Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug
	)
	{
		return ScriptsBinDirectory(runtimeRootDirectory, platform, configuration) /
			std::string(Constants::ScriptsDllFileName);
	}
}
