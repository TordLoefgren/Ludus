#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Engine::Persistence::Paths
{
	inline std::filesystem::path LudusRoot()
	{
		return Ludus::Engine::Platform::Paths::LocalAppData() / "Ludus";
	}

	inline std::filesystem::path ProjectsRoot()
	{
		return LudusRoot() / "Projects";
	}

	inline std::filesystem::path ProjectRoot(std::string_view projectName)
	{
		return ProjectsRoot() / std::string(projectName);
	}

	inline std::filesystem::path ProjectFile(std::string_view projectName)
	{
		return ProjectRoot(projectName) / (std::string(projectName) + ".lproj");
	}

	inline std::filesystem::path AssetsDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / "Assets";
	}

	inline std::filesystem::path ScenesDirectory(const std::filesystem::path& projectRoot)
	{
		return AssetsDirectory(projectRoot) / "Scenes";
	}

	inline std::filesystem::path SceneFile(const std::filesystem::path& projectRoot, std::string_view sceneName)
	{
		return ScenesDirectory(projectRoot) / (std::string(sceneName) + ".lscene");
	}

	inline void EnsureProjectsRootExists()
	{
		std::filesystem::create_directories(ProjectsRoot());
	}

	inline void EnsureProjectLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(ScenesDirectory(projectRoot));
	}
}
