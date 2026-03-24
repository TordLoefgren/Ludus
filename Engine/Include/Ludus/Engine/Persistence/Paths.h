#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace Ludus::Engine::Persistence::Paths
{
	namespace Constants
	{
		inline constexpr std::string_view AssetsDirectory = "Assets";
		inline constexpr std::string_view ScenesDirectory = "Scenes";
		inline constexpr std::string_view ScriptsDirectory = "Scripts";

		inline constexpr std::string_view ScriptsModuleFile = "Scripts.dll";

		inline constexpr std::string_view RuntimeManifestExtension = ".ludus.runtime";
		inline constexpr std::string_view SceneExtension = ".ludus.scene";
	}

	inline std::filesystem::path AssetsDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::AssetsDirectory);
	}

	inline std::filesystem::path ScenesDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::ScenesDirectory);
	}

	inline std::filesystem::path ScriptsDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::ScriptsDirectory);
	}

	inline std::filesystem::path SceneFile(const std::filesystem::path& runtimeRootDirectory, std::string_view sceneName)
	{
		return ScenesDirectory(runtimeRootDirectory) / (std::string(sceneName) + std::string(Constants::SceneExtension));
	}

	inline std::filesystem::path RuntimeManifestFile(std::string_view runtimeName)
	{
		return std::string(runtimeName) + std::string(Constants::RuntimeManifestExtension);
	}

	inline std::filesystem::path RuntimeManifestFile(const std::filesystem::path& runtimeRootDirectory, std::string_view runtimeName)
	{
		return runtimeRootDirectory / RuntimeManifestFile(runtimeName);
	}

	inline std::filesystem::path ScriptsDllFile(const std::filesystem::path& runtimeRootDirectory)
	{
		return ScriptsDirectory(runtimeRootDirectory) / std::string(Constants::ScriptsModuleFile);
	}
}
