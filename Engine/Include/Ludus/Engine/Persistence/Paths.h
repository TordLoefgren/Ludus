#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/Build/Platform.h>
#include <Ludus/Engine/Core/Enums.h>

namespace Ludus::Engine::Persistence::Paths
{
	namespace Constants
	{
		inline constexpr std::string_view LudusDirectoryName = "Ludus";
		inline constexpr std::string_view ProjectsDirectoryName = "Projects";
		inline constexpr std::string_view AssetsDirectoryName = "Assets";
		inline constexpr std::string_view ScenesDirectoryName = "Scenes";
		inline constexpr std::string_view ScriptsDirectoryName = "Scripts";
		inline constexpr std::string_view SourceDirectoryName = "Source";
		inline constexpr std::string_view BuildDirectoryName = "Build";
		inline constexpr std::string_view BinDirectoryName = "Bin";
		inline constexpr std::string_view ScriptsDllFileName = "Scripts.dll";
		inline constexpr std::string_view ProjectFileExtension = ".ludus.app";
		inline constexpr std::string_view SceneFileExtension = ".ludus.scene";
	}

	inline std::filesystem::path ProjectFile(std::string_view projectName)
	{
		return std::string(projectName) + std::string(Constants::ProjectFileExtension);
	}

	inline std::filesystem::path AssetsDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / std::string(Constants::AssetsDirectoryName);
	}

	inline std::filesystem::path ScenesDirectory(const std::filesystem::path& projectRoot)
	{
		return AssetsDirectory(projectRoot) / std::string(Constants::ScenesDirectoryName);
	}

	inline std::filesystem::path SceneFile(const std::filesystem::path& projectRoot, std::string_view sceneName)
	{
		return ScenesDirectory(projectRoot) / (std::string(sceneName) + std::string(Constants::SceneFileExtension));
	}

	inline std::filesystem::path ScriptsDllFile(
		const std::filesystem::path& projectRoot,
		const Ludus::Engine::Core::Build::Platform platform = Ludus::Engine::Core::Build::Platform::X64,
		const Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug
	)
	{
		return AssetsDirectory(projectRoot) /
			std::string(Constants::ScriptsDirectoryName) /
			std::string(Constants::BuildDirectoryName) /
			std::string(Constants::BinDirectoryName) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration)) /
			std::string(Constants::ScriptsDllFileName);
	}
}
