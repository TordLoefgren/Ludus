#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Editor::Persistence::Paths
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
		inline constexpr std::string_view ObjDirectoryName = "Obj";

		inline constexpr std::string_view EngineDirectoryName = "Engine";
		inline constexpr std::string_view IncludeDirectoryName = "Include";
		inline constexpr std::string_view ScriptingDirectoryName = "Scripting";
		inline constexpr std::string_view ApiDirectoryName = "API";
		inline constexpr std::string_view ResourcesDirectoryName = "Resources";
		inline constexpr std::string_view TemplatesDirectoryName = "Templates";

		inline constexpr std::string_view ScriptTargetName = "Scripts";
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

	inline std::filesystem::path ScriptsSourceDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot /
			std::string(Constants::AssetsDirectoryName) /
			std::string(Constants::ScriptsDirectoryName) /
			std::string(Constants::SourceDirectoryName);
	}

	inline std::filesystem::path ScriptsBuildDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot /
			std::string(Constants::AssetsDirectoryName) /
			std::string(Constants::ScriptsDirectoryName) /
			std::string(Constants::BuildDirectoryName);
	}

	inline std::filesystem::path ScriptsBinDirectory(const std::filesystem::path& projectRoot)
	{
		return ScriptsBuildDirectory(projectRoot) / std::string(Constants::BinDirectoryName);
	}

	inline std::filesystem::path ScriptsObjDirectory(const std::filesystem::path& projectRoot)
	{
		return ScriptsBuildDirectory(projectRoot) / std::string(Constants::ObjDirectoryName);
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

	inline std::filesystem::path ScenesDirectory(const std::filesystem::path& projectRoot)
	{
		return projectRoot /
			std::string(Constants::AssetsDirectoryName) /
			std::string(Constants::ScenesDirectoryName);
	}

	inline void EnsureProjectsRootExists()
	{
		std::filesystem::create_directories(ProjectsRoot());
	}

	inline void EnsureProjectScriptsLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(ScriptsSourceDirectory(projectRoot));
	}

	inline void EnsureProjectLayoutExists(const std::filesystem::path& projectRoot)
	{
		std::filesystem::create_directories(ScenesDirectory(projectRoot));
		EnsureProjectScriptsLayoutExists(projectRoot);
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
		const auto executableDirectory = Ludus::Engine::Platform::Paths::GetExecutablePath();
		auto root = FindAncestorWithEngineInclude(executableDirectory);

		if (root.empty())
		{
			throw std::runtime_error("Could not locate Ludus root (Engine/Include not found).");
		}

		return root / std::string(Constants::EngineDirectoryName) / std::string(Constants::IncludeDirectoryName);
	}

	inline std::filesystem::path EngineScriptingApiScriptsIncludeDir()
	{
		return ResolveEngineIncludeDir() /
			std::string(Constants::LudusDirectoryName) /
			std::string(Constants::EngineDirectoryName) /
			std::string(Constants::ScriptingDirectoryName) /
			std::string(Constants::ApiDirectoryName) /
			std::string(Constants::ScriptTargetName);
	}

	inline std::filesystem::path ScriptTemplatesDirectory()
	{
		const auto runtimeTemplateRoot = std::filesystem::current_path() /
			std::string(Constants::ResourcesDirectoryName) /
			std::string(Constants::ScriptingDirectoryName) /
			std::string(Constants::TemplatesDirectoryName);

		if (std::filesystem::exists(runtimeTemplateRoot))
		{
			return std::filesystem::canonical(runtimeTemplateRoot);
		}

		const auto devTemplateRoot = std::filesystem::path(__FILE__).parent_path() /
			".." / ".." / ".." / ".." /
			std::string(Constants::ResourcesDirectoryName) /
			std::string(Constants::ScriptingDirectoryName) /
			std::string(Constants::TemplatesDirectoryName);

		if (std::filesystem::exists(devTemplateRoot))
		{
			return std::filesystem::canonical(devTemplateRoot);
		}

		throw std::runtime_error("Script template directory was not found.");
	}
}
