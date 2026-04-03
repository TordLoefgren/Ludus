#include "pch.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildScriptPipeline.h>
#include <Ludus/Editor/Persistence/BuildPaths.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Editor/Persistence/RepositoryPaths.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Strings.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Platform/Process.h>

namespace
{
	struct Constants
	{
		static constexpr std::string_view ScriptCppExtension = ".cpp";
		static constexpr std::string_view ScriptSourceTemplateFile = "Script.cpp.template";
		static constexpr std::string_view ScriptsProjectTemplateFile = "Scripts.vcxproj.template";
		static constexpr std::string_view ScriptsModuleTemplateFile = "ScriptsModule.cpp.template";
		static constexpr std::string_view ScriptsProjectFileName = "Scripts.vcxproj";
		static constexpr std::string_view ScriptsModuleFileName = "ScriptsModule.cpp";

		static constexpr std::string_view AutoGenDeclareBegin = "// <LUDUS_AUTOGEN_DECLARE>";
		static constexpr std::string_view AutoGenDeclareEnd = "// </LUDUS_AUTOGEN_DECLARE>";
		static constexpr std::string_view AutoGenAddBegin = "// <LUDUS_AUTOGEN_ADD>";
		static constexpr std::string_view AutoGenAddEnd = "// </LUDUS_AUTOGEN_ADD>";
		static constexpr std::string_view AutoGenSourcesBegin = "<!-- <LUDUS_AUTOGEN_SOURCES> -->";
		static constexpr std::string_view AutoGenSourcesEnd = "<!-- </LUDUS_AUTOGEN_SOURCES> -->";

		static constexpr std::string_view ScriptNameToken = "SCRIPT_NAME";
		static constexpr std::string_view IncludeDirectoryToken = "${LUDUS_SCRIPTS_INCLUDE_DIR}";
		static constexpr std::string_view OutDirectoryToken = "${LUDUS_SCRIPT_OUT_DIR}";
		static constexpr std::string_view IntermediateDirectoryToken = "${LUDUS_SCRIPT_INT_DIR}";
		static constexpr std::string_view ScriptingProjectPathToken = "${LUDUS_SCRIPTING_PROJECT_PATH}";
		static constexpr std::string_view TargetNameToken = "${LUDUS_TARGET_NAME}";

		static constexpr std::string_view BuildCommand = "Build";
		static constexpr std::string_view RebuildCommand = "Rebuild";
		static constexpr std::string_view CleanCommand = "Clean";
	};

	void RemoveProjectScriptsBuildLayout(const std::filesystem::path& projectRoot)
	{
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::ScriptsBinDirectory(projectRoot));
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::ScriptsObjDirectory(projectRoot));

		Ludus::Engine::FileSystem::RemoveDirectoryIfEmpty(Ludus::Editor::Persistence::BuildPaths::BinDirectory(projectRoot));
		Ludus::Engine::FileSystem::RemoveDirectoryIfEmpty(Ludus::Editor::Persistence::BuildPaths::ObjDirectory(projectRoot));
	}
}

namespace Ludus::Editor::Build::MSBuild
{
	void MSBuildScriptPipeline::CopyTemplateToDestinationIfMissing(
		const std::filesystem::path& templateRoot,
		std::string_view templateFileName,
		const std::filesystem::path& destinationPath
	) const
	{
		if (std::filesystem::exists(destinationPath))
		{
			return;
		}

		const auto sourcePath = templateRoot / std::string(templateFileName);
		const auto text = Ludus::Engine::FileSystem::ReadAllText(sourcePath);
		Ludus::Engine::FileSystem::WriteAllText(destinationPath, text);
	}

	void MSBuildScriptPipeline::AddScript(std::string_view name, const std::filesystem::path& projectRoot)
	{
		const auto templateRoot = Ludus::Editor::Persistence::RepositoryPaths::ScriptTemplatesDirectory();
		const auto scriptSourceDirectory = Ludus::Editor::Persistence::ProjectPaths::ScriptsSourceDirectory(projectRoot);
		const auto destinationPath = scriptSourceDirectory / (std::string(name) + std::string(Constants::ScriptCppExtension));

		if (std::filesystem::exists(destinationPath))
		{
			throw std::runtime_error("Script already exists: " + destinationPath.string());
		}

		const auto sourcePath = templateRoot / std::string(Constants::ScriptSourceTemplateFile);
		auto text = Ludus::Engine::FileSystem::ReadAllText(sourcePath);

		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::ScriptNameToken, name);
		Ludus::Engine::FileSystem::WriteAllText(destinationPath, text);
	}

	void MSBuildScriptPipeline::AddScriptsModuleReference(std::string_view name, const std::filesystem::path& projectRoot)
	{
		const auto scriptModulePath = Ludus::Editor::Persistence::ProjectPaths::ScriptsModuleFile(projectRoot);
		auto text = Ludus::Engine::FileSystem::ReadAllText(scriptModulePath);

		Ludus::Engine::Core::Strings::UpsertLineInRegion(
			text,
			Constants::AutoGenDeclareBegin,
			Constants::AutoGenDeclareEnd,
			"LUDUS_FORWARD_DECLARE_SCRIPT(" + std::string(name) + ");"
		);

		Ludus::Engine::Core::Strings::UpsertLineInRegion(
			text,
			Constants::AutoGenAddBegin,
			Constants::AutoGenAddEnd,
			"    Add_" + std::string(name) + "(registry);"
		);

		Ludus::Engine::FileSystem::WriteAllText(scriptModulePath, text);
	}

	void MSBuildScriptPipeline::AddScriptsProjectReference(
		std::string_view name,
		const std::filesystem::path& projectRoot
	)
	{
		const auto projectPath = Ludus::Editor::Persistence::ProjectPaths::ScriptsProjectFile(projectRoot);
		auto text = Ludus::Engine::FileSystem::ReadAllText(projectPath);

		Ludus::Engine::Core::Strings::UpsertLineInRegion(
			text,
			Constants::AutoGenSourcesBegin,
			Constants::AutoGenSourcesEnd,
			"\t\t<ClCompile Include=\"" + std::string(name) + std::string(Constants::ScriptCppExtension) + "\" />"
		);

		Ludus::Engine::FileSystem::WriteAllText(projectPath, text);
	}

	void MSBuildScriptPipeline::EnsureBuildFiles(const std::filesystem::path& projectRoot)
	{
		Ludus::Editor::Persistence::ProjectPaths::EnsureProjectScriptsSourceLayoutExists(projectRoot);

		const auto templateRoot = Ludus::Editor::Persistence::RepositoryPaths::ScriptTemplatesDirectory();
		const auto scriptSourceDirectory = Ludus::Editor::Persistence::ProjectPaths::ScriptsSourceDirectory(projectRoot);

		CopyTemplateToDestinationIfMissing(
			templateRoot,
			Constants::ScriptsProjectTemplateFile,
			scriptSourceDirectory / std::string(Constants::ScriptsProjectFileName)
		);

		CopyTemplateToDestinationIfMissing(
			templateRoot,
			Constants::ScriptsModuleTemplateFile,
			scriptSourceDirectory / std::string(Constants::ScriptsModuleFileName)
		);
	}

	void MSBuildScriptPipeline::SetScriptProjectCompilationSettings(
		const std::filesystem::path& projectRoot,
		const ScriptBuildSettings& settings
	)
	{
		const auto projectPath = Ludus::Editor::Persistence::ProjectPaths::ScriptsProjectFile(projectRoot);
		auto text = Ludus::Engine::FileSystem::ReadAllText(projectPath);

		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::IncludeDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.APIIncludeDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::OutDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.OutDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::IntermediateDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.InDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::ScriptingProjectPathToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.ScriptingProjectPath));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::TargetNameToken, settings.TargetName);

		if (text.find("${") != std::string::npos)
		{
			throw std::runtime_error("Scripts.vcxproj still contains unresolved ${...} placeholders.");
		}

		Ludus::Engine::FileSystem::WriteAllText(projectPath, text);
	}

	MSBuildScriptPipeline::MSBuildScriptPipeline(MSBuildContext& context)
		: m_Context(context)
	{ }

	void MSBuildScriptPipeline::RunBuild(
		const std::filesystem::path& projectRoot,
		BuildConfiguration configuration,
		BuildCommand command
	)
	{
		if (command == BuildCommand::Build || command == BuildCommand::Rebuild)
		{
			Ludus::Editor::Persistence::BuildPaths::EnsureProjectScriptsBuildLayoutExists(projectRoot);
		}

		if (!m_Context.MSBuildPath)
		{
			throw std::runtime_error("MSBuild path not found.");
		}

		const auto projectPath = Ludus::Editor::Persistence::ProjectPaths::ScriptsProjectFile(projectRoot);

		const auto configurationStr = Ludus::Engine::Core::Enums::GetDisplayName(configuration);
		const auto platformStr = Ludus::Engine::Core::Enums::GetDisplayName(BuildPlatform::WindowsX64);
		const auto commandStr = command == BuildCommand::Build
			? Constants::BuildCommand
			: command == BuildCommand::Rebuild ? Constants::RebuildCommand : Constants::CleanCommand;

		std::string args =
			"\"" + Ludus::Engine::FileSystem::ToPortablePathString(projectPath) + "\" "
			"/m "
			"/t:" + std::string(commandStr) + " "
			"/p:Configuration=" + std::string(configurationStr) + " "
			"/p:Platform=" + std::string(platformStr) + " "
			"/nologo";

		const auto expected = Ludus::Engine::Platform::Process::Run(*m_Context.MSBuildPath, args);
		if (!expected.HasValue())
		{
			LUDUS_LOG_ERROR("Process error: " + std::string(expected.GetError().what()));
			return;
		}

		LUDUS_LOG_DEBUG(std::string(commandStr) + " executed...");

		const auto& result = expected.GetValue();
		if (result.ExitCode != 0)
		{
			LUDUS_LOG_ERROR("MSBuild failed with exit code: " + std::to_string(result.ExitCode));
			LUDUS_LOG_ERROR(result.Output);

			return;
		}

		if (command == BuildCommand::Clean)
		{
			RemoveProjectScriptsBuildLayout(projectRoot);
		}
	}

	void MSBuildScriptPipeline::EnsureScriptProject(
		const std::filesystem::path& projectRoot,
		const ScriptBuildSettings& settings
	)
	{
		EnsureBuildFiles(projectRoot);
		SetScriptProjectCompilationSettings(projectRoot, settings);
	}

	void MSBuildScriptPipeline::CreateScript(
		const std::filesystem::path& projectRoot,
		std::string_view name
	)
	{
		AddScript(name, projectRoot);
		AddScriptsModuleReference(name, projectRoot);
		AddScriptsProjectReference(name, projectRoot);
	}
}
