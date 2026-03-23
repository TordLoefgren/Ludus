#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/Build/OperatingSystem.h>
#include <Ludus/Engine/Core/Build/Platform.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Editor::Persistence::Paths
{
	namespace Constants
	{
		inline constexpr std::string_view LudusDirectoryName = "Ludus";
		inline constexpr std::string_view ProjectsDirectoryName = "Projects";
		inline constexpr std::string_view SourceDirectoryName = "Source";
		inline constexpr std::string_view ObjDirectoryName = "Obj";
		inline constexpr std::string_view ProjectFileExtension = ".ludus.project";

		inline constexpr std::string_view EngineDirectoryName = "Engine";
		inline constexpr std::string_view IncludeDirectoryName = "Include";
		inline constexpr std::string_view LudusIncludeDirectoryName = "Ludus";
		inline constexpr std::string_view EngineIncludeDirectoryName = "Engine";
		inline constexpr std::string_view ScriptingIncludeDirectoryName = "Scripting";
		inline constexpr std::string_view ApiDirectoryName = "API";
		inline constexpr std::string_view ResourcesDirectoryName = "Resources";
		inline constexpr std::string_view ScriptingResourcesDirectoryName = "Scripting";
		inline constexpr std::string_view TemplatesDirectoryName = "Templates";

		inline constexpr std::string_view ScriptTargetName = "Scripts";
		inline constexpr std::string_view InvalidFileNameCharacters = "<>:\"/\\|?*";
	}

	inline std::filesystem::path LudusRoot()
	{
		return Ludus::Engine::Platform::Paths::LocalAppData() / std::string(Constants::LudusDirectoryName);
	}

	inline std::filesystem::path ProjectsRoot()
	{
		return LudusRoot() / std::string(Constants::ProjectsDirectoryName);
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
		return std::string(projectName) + std::string(Constants::ProjectFileExtension);
	}

	inline std::filesystem::path ProjectManifestFile(const std::filesystem::path& projectRoot, std::string_view projectName)
	{
		return projectRoot / ProjectManifestFile(projectName);
	}

	inline std::filesystem::path BinDirectory(const std::filesystem::path& projectRoot)
	{
		return Ludus::Engine::Persistence::Paths::BinDirectory(projectRoot);
	}

	inline std::filesystem::path ObjDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot / std::string(Constants::ObjDirectoryName);
	}

	inline std::filesystem::path BuildsDirectory(const std::filesystem::path& projectRoot)
	{
		return Ludus::Engine::Persistence::Paths::BuildsDirectory(projectRoot);
	}

	inline std::filesystem::path BuildOutputDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Engine::Core::Build::OperatingSystem operatingSystem = Ludus::Engine::Core::Build::OperatingSystem::Windows,
		const Ludus::Engine::Core::Build::Platform platform = Ludus::Engine::Core::Build::Platform::WindowsX64,
		const Ludus::Engine::Core::Build::Configuration configuration = Ludus::Engine::Core::Build::Configuration::Debug
	)
	{
		return Ludus::Engine::Persistence::Paths::BuildOutputDirectory(
			projectRoot,
			operatingSystem,
			platform,
			configuration
		);
	}

	inline std::filesystem::path ScriptsSourceDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot /
			std::string(Ludus::Engine::Persistence::Paths::Constants::ScriptsDirectoryName) /
			std::string(Constants::SourceDirectoryName);
	}

	inline std::filesystem::path ScriptsBinDirectory(const std::filesystem::path& projectRoot)
	{
		return Ludus::Engine::Persistence::Paths::ScriptsBinDirectory(projectRoot);
	}

	inline std::filesystem::path ScriptsBinDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Engine::Core::Build::Platform platform,
		const Ludus::Engine::Core::Build::Configuration configuration
	)
	{
		return Ludus::Engine::Persistence::Paths::ScriptsBinDirectory(projectRoot, platform, configuration);
	}

	inline std::filesystem::path ScriptsObjDirectory(const std::filesystem::path& projectRoot)
	{
		return ObjDirectory(projectRoot) / std::string(Ludus::Engine::Persistence::Paths::Constants::ScriptsDirectoryName);
	}

	inline std::filesystem::path ScriptsObjDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Engine::Core::Build::Platform platform,
		const Ludus::Engine::Core::Build::Configuration configuration
	)
	{
		return ScriptsObjDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	inline std::filesystem::path ScriptSourceFile(const std::filesystem::path& projectRoot, std::string_view scriptName)
	{
		return ScriptsSourceDirectory(projectRoot) / (std::string(scriptName) + ".cpp");
	}

	inline std::filesystem::path ScriptsModuleFile(const std::filesystem::path& projectRoot)
	{
		return ScriptsSourceDirectory(projectRoot) / "ScriptsModule.cpp";
	}

	inline std::filesystem::path ScriptsProjectFile(const std::filesystem::path& projectRoot)
	{
		return ScriptsSourceDirectory(projectRoot) / "Scripts.vcxproj";
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

	inline void EnsureProjectScriptsBuildLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(ScriptsBinDirectory(projectRoot));
		std::filesystem::create_directories(ScriptsObjDirectory(projectRoot));
	}

	inline void EnsureProjectScriptsLayoutExists(const std::filesystem::path& projectRoot)
	{
		EnsureProjectScriptsSourceLayoutExists(projectRoot);
		EnsureProjectScriptsBuildLayoutExists(projectRoot);
	}

	inline void EnsureProjectLayoutExists(const std::filesystem::path& projectRoot)
	{
		EnsureProjectRootExists(projectRoot);
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot));
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot));
	}

	inline std::filesystem::path FindAncestorWithEngineInclude(std::filesystem::path startPath)
	{
		for (auto path = startPath; !path.empty(); path = path.parent_path())
		{
			if (std::filesystem::exists(
				path / std::string(Constants::EngineDirectoryName) / std::string(Constants::IncludeDirectoryName)
			))
			{
				return path;
			}
		}

		return { };
	}

	inline std::filesystem::path ResolveEngineIncludeDir()
	{
		const auto executableDirectory = Ludus::Engine::Platform::Paths::GetExecutablePath().parent_path();
		auto root = FindAncestorWithEngineInclude(executableDirectory);

		if (root.empty())
		{
			throw std::runtime_error("Could not locate Ludus root (Engine/Include not found).");
		}

		return root / std::string(Constants::EngineDirectoryName) / std::string(Constants::IncludeDirectoryName);
	}

	inline std::filesystem::path EngineScriptingApiScriptsIncludeDir()
	{
		const auto engineIncludeDirectory = ResolveEngineIncludeDir();

		return engineIncludeDirectory /
			std::string(Constants::LudusIncludeDirectoryName) /
			std::string(Constants::EngineIncludeDirectoryName) /
			std::string(Constants::ScriptingIncludeDirectoryName) /
			std::string(Constants::ApiDirectoryName) /
			std::string(Constants::ScriptTargetName);
	}

	inline std::filesystem::path ScriptTemplatesDirectory()
	{
		const auto runtimeTemplateRoot = std::filesystem::current_path() /
			std::string(Constants::ResourcesDirectoryName) /
			std::string(Constants::ScriptingResourcesDirectoryName) /
			std::string(Constants::TemplatesDirectoryName);

		if (std::filesystem::exists(runtimeTemplateRoot))
		{
			return std::filesystem::canonical(runtimeTemplateRoot);
		}

		const auto devTemplateRoot = std::filesystem::path(__FILE__).parent_path() /
			".." / ".." / ".." / ".." /
			std::string(Constants::ResourcesDirectoryName) /
			std::string(Constants::ScriptingResourcesDirectoryName) /
			std::string(Constants::TemplatesDirectoryName);

		if (std::filesystem::exists(devTemplateRoot))
		{
			return std::filesystem::canonical(devTemplateRoot);
		}

		throw std::runtime_error("Script template directory was not found.");
	}
}
