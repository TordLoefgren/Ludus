#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

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

	inline std::filesystem::path ResolveRuntimeScenePath(
		const std::filesystem::path& runtimeRootDirectory,
		const std::filesystem::path& scenePath
	)
	{
		return Ludus::Engine::FileSystem::ResolvePathFromRoot(runtimeRootDirectory, scenePath);
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

	inline bool IsValidRuntimeAssetPath(const std::filesystem::path& path)
	{
		return Ludus::Engine::FileSystem::IsRelativePathUnderDirectory(path, Constants::AssetsDirectory);
	}

	inline bool IsValidRuntimeScenePath(const std::filesystem::path& path)
	{
		if (!Ludus::Engine::FileSystem::IsRelativePathUnderDirectory(path, Constants::ScenesDirectory))
		{
			return false;
		}

		return Ludus::Engine::FileSystem::HasLogicalExtension(path, Constants::SceneExtension);
	}

	inline std::filesystem::path NormalizeRuntimeAssetPathOrEmpty(
		const std::filesystem::path& runtimeRootDirectory,
		const std::filesystem::path& path
	)
	{
		const auto normalizedPath = Ludus::Engine::FileSystem::NormalizePathRelativeToRootOrEmpty(runtimeRootDirectory, path);

		if (!IsValidRuntimeAssetPath(normalizedPath))
		{
			return { };
		}

		return normalizedPath;
	}

	inline std::filesystem::path NormalizeRuntimeScenePathOrEmpty(
		const std::filesystem::path& runtimeRootDirectory,
		const std::filesystem::path& path
	)
	{
		const auto normalizedPath = Ludus::Engine::FileSystem::NormalizePathRelativeToRootOrEmpty(runtimeRootDirectory, path);

		if (!IsValidRuntimeScenePath(normalizedPath))
		{
			return { };
		}

		return normalizedPath;
	}

	inline void ValidateRuntimeManifestPaths(const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest)
	{
		for (const auto& asset : runtimeManifest.Assets)
		{
			if (!IsValidRuntimeAssetPath(asset.Path))
			{
				throw std::runtime_error(
					"Runtime manifest contains an asset path outside the runtime Assets directory."
				);
			}
		}

		for (const auto& scene : runtimeManifest.Scenes)
		{
			if (!IsValidRuntimeScenePath(scene.Path))
			{
				throw std::runtime_error(
					"Runtime manifest contains a scene path outside the runtime Scenes directory."
				);
			}
		}
	}
}
