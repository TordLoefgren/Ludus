#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Editor::Persistence::RepositoryPaths
{
	namespace Constants
	{
		inline constexpr std::string_view EditorDirectory = "Editor";
		inline constexpr std::string_view EngineDirectory = "Engine";
		inline constexpr std::string_view IncludeDirectory = "Include";
		inline constexpr std::string_view ScriptingDirectory = "Scripting";
		inline constexpr std::string_view ScriptsDirectory = "Scripts";
		inline constexpr std::string_view RuntimeHostDirectory = "RuntimeHost";
		inline constexpr std::string_view ApiDirectory = "API";
		inline constexpr std::string_view ResourcesDirectory = "Resources";
		inline constexpr std::string_view VendorsDirectory = "Vendors";
		inline constexpr std::string_view TemplatesDirectory = "Templates";
		inline constexpr std::string_view LudusDirectory = "Ludus";
	}

	inline std::filesystem::path ResolveRepositoryRoot()
	{
		const auto executableDirectory = Ludus::Engine::Platform::Paths::GetExecutablePath();
		for (auto path = executableDirectory; !path.empty(); path = path.parent_path())
		{
			if (std::filesystem::exists(path / ".git") || std::filesystem::exists(path / "Ludus.sln"))
			{
				return path;
			}
		}

		throw std::runtime_error("Repository root could not be resolved.");
	}

	inline std::filesystem::path EditorDirectory()
	{
		return ResolveRepositoryRoot() / std::string(Constants::EditorDirectory);
	}

	inline std::filesystem::path EditorIncludeDirectory()
	{
		return EditorDirectory() / std::string(Constants::IncludeDirectory);
	}

	inline std::filesystem::path EditorResourcesDirectory()
	{
		return EditorDirectory() / std::string(Constants::ResourcesDirectory);
	}

	inline std::filesystem::path EngineDirectory()
	{
		return ResolveRepositoryRoot() / std::string(Constants::EngineDirectory);
	}

	inline std::filesystem::path EngineIncludeDirectory()
	{
		return EngineDirectory() / std::string(Constants::IncludeDirectory);
	}

	inline std::filesystem::path EngineResourcesDirectory()
	{
		return EngineDirectory() / std::string(Constants::ResourcesDirectory);
	}

	inline std::filesystem::path EngineVendorsDirectory()
	{
		return EngineDirectory() / std::string(Constants::VendorsDirectory);
	}

	inline std::filesystem::path EngineScriptingApiScriptsIncludeDirectory()
	{
		return EngineIncludeDirectory() /
			std::string(Constants::LudusDirectory) /
			std::string(Constants::EngineDirectory) /
			std::string(Constants::ScriptingDirectory) /
			std::string(Constants::ApiDirectory) /
			std::string(Constants::ScriptsDirectory);
	}

	inline std::filesystem::path TemplatesDirectory(std::string_view templateDirectory)
	{
		return EditorResourcesDirectory() /
			std::string(Constants::TemplatesDirectory) /
			std::string(templateDirectory);
	}

	inline std::filesystem::path RuntimeHostTemplatesDirectory()
	{
		return TemplatesDirectory(Constants::RuntimeHostDirectory);
	}

	inline std::filesystem::path ScriptTemplatesDirectory()
	{
		return TemplatesDirectory(Constants::ScriptingDirectory);
	}
}
