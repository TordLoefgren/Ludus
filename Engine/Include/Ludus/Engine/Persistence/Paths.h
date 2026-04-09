#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace Ludus::Engine::Persistence::Paths
{
	namespace Constants
	{
		inline constexpr std::string_view AssetsDirectory = "Assets";
		inline constexpr std::string_view ResourcesDirectory = "Resources";
		inline constexpr std::string_view ScenesDirectory = "Scenes";
		inline constexpr std::string_view ShadersDirectory = "Shaders";
		inline constexpr std::string_view FontsDirectory = "Fonts";
		inline constexpr std::string_view LiberationSansDirectory = "liberation-sans";
		inline constexpr std::string_view DefaultFontFile = "LiberationSans-Regular.ttf";

		inline constexpr std::string_view ScriptsModuleFile = "Scripts.dll";

		inline constexpr std::string_view RuntimeManifestExtension = ".runtime.ludus";
		inline constexpr std::string_view RuntimeLaunchSettingsExtension = ".settings.ludus";
		inline constexpr std::string_view SceneExtension = ".scene.ludus";
	}

	inline std::filesystem::path AssetsDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::AssetsDirectory);
	}

	inline std::filesystem::path ResourcesDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::ResourcesDirectory);
	}

	inline std::filesystem::path ScenesDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::ScenesDirectory);
	}

	inline std::filesystem::path SceneFile(const std::filesystem::path& runtimeRootDirectory, std::string_view sceneName)
	{
		return ScenesDirectory(runtimeRootDirectory) / (std::string(sceneName) + std::string(Constants::SceneExtension));
	}

	inline std::filesystem::path RuntimeSceneFile(const std::filesystem::path& runtimeRootDirectory, const std::filesystem::path& scenePath)
	{
		return ScenesDirectory(runtimeRootDirectory) / scenePath.filename();
	}

	inline std::filesystem::path RuntimeRelativeSceneFile(const std::filesystem::path& scenePath)
	{
		return std::filesystem::path(std::string(Constants::ScenesDirectory)) / scenePath.filename();
	}

	inline std::filesystem::path ResolveRuntimeScenePath(
		const std::filesystem::path& runtimeRootDirectory,
		const std::filesystem::path& scenePath
	)
	{
		return scenePath.is_absolute() ? scenePath : runtimeRootDirectory / scenePath;
	}

	inline std::filesystem::path ShadersDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return ResourcesDirectory(runtimeRootDirectory) / std::string(Constants::ShadersDirectory);
	}

	inline std::filesystem::path FontsDirectory(const std::filesystem::path& runtimeRootDirectory)
	{
		return ResourcesDirectory(runtimeRootDirectory) / std::string(Constants::FontsDirectory);
	}

	inline std::filesystem::path DefaultFontFile(const std::filesystem::path& runtimeRootDirectory)
	{
		return FontsDirectory(runtimeRootDirectory) /
			std::string(Constants::LiberationSansDirectory) /
			std::string(Constants::DefaultFontFile);
	}

	inline std::filesystem::path RuntimeManifestFile(std::string_view runtimeName)
	{
		return std::string(runtimeName) + std::string(Constants::RuntimeManifestExtension);
	}

	inline std::filesystem::path RuntimeManifestFile(const std::filesystem::path& runtimeRootDirectory, std::string_view runtimeName)
	{
		return runtimeRootDirectory / RuntimeManifestFile(runtimeName);
	}

	inline std::filesystem::path RuntimeLaunchSettingsFile(std::string_view runtimeName)
	{
		return std::string(runtimeName) + std::string(Constants::RuntimeLaunchSettingsExtension);
	}

	inline std::filesystem::path RuntimeLaunchSettingsFile(const std::filesystem::path& runtimeRootDirectory, std::string_view runtimeName)
	{
		return runtimeRootDirectory / RuntimeLaunchSettingsFile(runtimeName);
	}

	inline std::filesystem::path ScriptsDllFile(const std::filesystem::path& runtimeRootDirectory)
	{
		return runtimeRootDirectory / std::string(Constants::ScriptsModuleFile);
	}
}
