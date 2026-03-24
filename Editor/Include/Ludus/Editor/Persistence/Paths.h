#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildOperatingSystem.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Editor::Persistence::Paths
{
	namespace Constants
	{
		inline constexpr std::string_view LudusRootDirectory = "Ludus";
		inline constexpr std::string_view ProjectsDirectory = "Projects";

		inline constexpr std::string_view SourceDirectory = "Source";
		inline constexpr std::string_view BinDirectory = "Bin";
		inline constexpr std::string_view ObjDirectory = "Obj";
		inline constexpr std::string_view BuildsDirectory = "Builds";
		inline constexpr std::string_view InvalidFileNameCharacters = "<>:\"/\\|?*";

		inline constexpr std::string_view ScriptsModuleSourceFile = "ScriptsModule.cpp";
		inline constexpr std::string_view ScriptsProjectFile = "Scripts.vcxproj";
		inline constexpr std::string_view ScriptsTarget = "Scripts";

		inline constexpr std::string_view CppExtension = ".cpp";
		inline constexpr std::string_view ProjectManifestExtension = ".ludus.project";

		inline constexpr std::string_view EngineDirectory = "Engine";
		inline constexpr std::string_view IncludeDirectory = "Include";
		inline constexpr std::string_view LudusIncludeDirectory = "Ludus";
		inline constexpr std::string_view EngineIncludeDirectory = "Engine";
		inline constexpr std::string_view ScriptingDirectory = "Scripting";
		inline constexpr std::string_view ApiDirectory = "API";
		inline constexpr std::string_view ResourcesDirectory = "Resources";
		inline constexpr std::string_view TemplatesDirectory = "Templates";
	}

	inline std::filesystem::path LudusRoot()
	{
		return Ludus::Engine::Platform::Paths::LocalAppData() / std::string(Constants::LudusRootDirectory);
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

	// Editor projects contain the runtime layout at the root and editor-only script build output alongside it.
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

	inline std::filesystem::path BuildOutputDirectory(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildOperatingSystem operatingSystem = Ludus::Editor::Build::BuildOperatingSystem::Windows,
		const Ludus::Editor::Build::BuildPlatform platform = Ludus::Editor::Build::BuildPlatform::WindowsX64,
		const Ludus::Editor::Build::BuildConfiguration configuration = Ludus::Editor::Build::BuildConfiguration::Debug
	)
	{
		return BuildsDirectory(projectRoot) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(operatingSystem)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(platform)) /
			std::string(Ludus::Engine::Core::Enums::GetDisplayName(configuration));
	}

	// Runtime-facing scripts live under the root Scripts directory, while editor-only source lives in Scripts/Source.
	inline std::filesystem::path ScriptsSourceDirectory(const std::filesystem::path& projectRoot)
	{
		return Ludus::Engine::Persistence::Paths::ScriptsDirectory(projectRoot) /
			std::string(Constants::SourceDirectory);
	}

	inline std::filesystem::path ScriptsBinDirectory(const std::filesystem::path& projectRoot)
	{
		return BinDirectory(projectRoot) / std::string(Ludus::Engine::Persistence::Paths::Constants::ScriptsDirectory);
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
		return ObjDirectory(projectRoot) / std::string(Ludus::Engine::Persistence::Paths::Constants::ScriptsDirectory);
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

	inline std::filesystem::path ScriptsDllFile(
		const std::filesystem::path& projectRoot,
		const Ludus::Editor::Build::BuildPlatform platform = Ludus::Editor::Build::BuildPlatform::WindowsX64,
		const Ludus::Editor::Build::BuildConfiguration configuration = Ludus::Editor::Build::BuildConfiguration::Debug
	)
	{
		return ScriptsBinDirectory(projectRoot, platform, configuration) /
			std::string(Ludus::Engine::Persistence::Paths::Constants::ScriptsModuleFile);
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
		std::filesystem::create_directories(Ludus::Engine::Persistence::Paths::ScriptsDirectory(projectRoot));
	}

	inline std::filesystem::path FindAncestorWithEngineInclude(std::filesystem::path startPath)
	{
		for (auto path = startPath; !path.empty(); path = path.parent_path())
		{
			if (std::filesystem::exists(
				path / std::string(Constants::EngineDirectory) / std::string(Constants::IncludeDirectory)
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

		return root / std::string(Constants::EngineDirectory) / std::string(Constants::IncludeDirectory);
	}

	inline std::filesystem::path EngineScriptingApiScriptsIncludeDir()
	{
		const auto engineIncludeDirectory = ResolveEngineIncludeDir();

		return engineIncludeDirectory /
			std::string(Constants::LudusIncludeDirectory) /
			std::string(Constants::EngineIncludeDirectory) /
			std::string(Constants::ScriptingDirectory) /
			std::string(Constants::ApiDirectory) /
			std::string(Constants::ScriptsTarget);
	}

	inline std::filesystem::path ScriptTemplatesDirectory()
	{
		const auto runtimeTemplateRoot = std::filesystem::current_path() /
			std::string(Constants::ResourcesDirectory) /
			std::string(Constants::ScriptingDirectory) /
			std::string(Constants::TemplatesDirectory);

		if (std::filesystem::exists(runtimeTemplateRoot))
		{
			return std::filesystem::canonical(runtimeTemplateRoot);
		}

		const auto devTemplateRoot = std::filesystem::path(__FILE__).parent_path() /
			".." / ".." / ".." / ".." /
			std::string(Constants::ResourcesDirectory) /
			std::string(Constants::ScriptingDirectory) /
			std::string(Constants::TemplatesDirectory);

		if (std::filesystem::exists(devTemplateRoot))
		{
			return std::filesystem::canonical(devTemplateRoot);
		}

		throw std::runtime_error("Script template directory was not found.");
	}
}
