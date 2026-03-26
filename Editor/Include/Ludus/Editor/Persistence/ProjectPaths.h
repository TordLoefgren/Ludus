#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Editor::Persistence::ProjectPaths
{
	namespace Constants
	{
		inline constexpr std::string_view LudusDirectory = "Ludus";
		inline constexpr std::string_view ProjectsDirectory = "Projects";
		inline constexpr std::string_view SourceDirectory = "Source";
		inline constexpr std::string_view InvalidFileNameCharacters = "<>:\"/\\|?*";
		inline constexpr std::string_view ScriptsModuleSourceFile = "ScriptsModule.cpp";
		inline constexpr std::string_view ScriptsProjectFile = "Scripts.vcxproj";
		inline constexpr std::string_view CppExtension = ".cpp";
		inline constexpr std::string_view ProjectManifestExtension = ".ludus.project";
		inline constexpr std::string_view ScriptsDirectory = "Scripts";
	}

	inline std::filesystem::path LudusRoot()
	{
		return Ludus::Engine::Platform::Paths::LocalAppData() / std::string(Constants::LudusDirectory);
	}

	inline std::filesystem::path ProjectsRoot()
	{
		return LudusRoot() / std::string(Constants::ProjectsDirectory);
	}

	inline std::filesystem::path ProjectRoot(std::string_view projectName)
	{
		return ProjectsRoot() / std::string(projectName);
	}

	inline std::string ValidateProjectName(std::string_view projectName)
	{
		if (projectName.empty())
		{
			return "Name must not be empty.";
		}

		if (projectName.find_first_of(Constants::InvalidFileNameCharacters) != std::string_view::npos)
		{
			return "Name contains invalid path characters.";
		}

		return { };
	}

	inline std::string ValidateScriptName(std::string_view scriptName)
	{
		if (scriptName.empty())
		{
			return "Name must not be empty.";
		}

		if (scriptName.find_first_of(Constants::InvalidFileNameCharacters) != std::string_view::npos)
		{
			return "Name contains invalid path characters.";
		}

		return { };
	}

	inline std::string ValidateProjectDirectory(const std::filesystem::path& projectDirectory)
	{
		if (projectDirectory.empty())
		{
			return "Project directory is invalid.";
		}

		std::error_code errorCode;
		const auto parentDirectory = projectDirectory.parent_path();
		if (parentDirectory.empty())
		{
			return "Project directory is invalid.";
		}

		std::filesystem::create_directories(parentDirectory, errorCode);
		if (errorCode)
		{
			return "Project directory is not accessible.";
		}

		if (std::filesystem::exists(projectDirectory, errorCode))
		{
			if (errorCode)
			{
				return "Project directory could not be validated.";
			}

			return "Project directory already exists.";
		}

		return { };
	}

	inline std::filesystem::path ProjectManifestFile(std::string_view projectName)
	{
		return std::string(projectName) + std::string(Constants::ProjectManifestExtension);
	}

	inline std::filesystem::path ProjectManifestFile(const std::filesystem::path& projectRoot, std::string_view projectName)
	{
		return projectRoot / ProjectManifestFile(projectName);
	}

	inline std::filesystem::path ScriptsDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / std::string(Constants::ScriptsDirectory);
	}

	inline std::filesystem::path ScriptsSourceDirectory(const std::filesystem::path& projectRoot)
	{
		return ScriptsDirectory(projectRoot) / std::string(Constants::SourceDirectory);
	}

	inline std::filesystem::path ScriptSourceFile(const std::filesystem::path& projectRoot, std::string_view scriptName)
	{
		return ScriptsSourceDirectory(projectRoot) / (std::string(scriptName) + std::string(Constants::CppExtension));
	}

	inline std::filesystem::path ScriptsModuleFile(const std::filesystem::path& projectRoot)
	{
		return ScriptsSourceDirectory(projectRoot) / std::string(Constants::ScriptsModuleSourceFile);
	}

	inline std::filesystem::path ScriptsProjectFile(const std::filesystem::path& projectRoot)
	{
		return ScriptsSourceDirectory(projectRoot) / std::string(Constants::ScriptsProjectFile);
	}

	inline void EnsureProjectsRootExists()
	{
		std::filesystem::create_directories(ProjectsRoot());
	}

	inline void EnsureProjectRootExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(projectRoot);
	}

	inline void EnsureProjectScriptsSourceLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(ScriptsSourceDirectory(projectRoot));
	}

	inline void EnsureProjectLayoutExists(const std::filesystem::path& projectRoot)
	{
		EnsureProjectRootExists(projectRoot);
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot));
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot));
		std::filesystem::create_directories(ScriptsDirectory(projectRoot));
	}
}
