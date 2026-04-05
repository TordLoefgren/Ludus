#include "pch.h"

#include <stdexcept>
#include <string>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildRuntimeHostPipeline.h>
#include <Ludus/Editor/Build/RuntimeManifestBuildHelpers.h>
#include <Ludus/Editor/Persistence/BuildPaths.h>
#include <Ludus/Editor/Persistence/RepositoryPaths.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Strings.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/LmlRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Guid.h>
#include <Ludus/Engine/Platform/Process.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace
{
	struct Constants
	{
		static constexpr std::string_view RuntimeHostProjectTemplateFile = "RuntimeHost.vcxproj.template";
		static constexpr std::string_view RuntimeHostModuleTemplateFile = "RuntimeHostModule.cpp.template";
		static constexpr std::string_view RuntimeHostProjectFileName = "RuntimeHost.vcxproj";
		static constexpr std::string_view RuntimeHostModuleFileName = "RuntimeHostModule.cpp";

		static constexpr std::string_view OutDirectoryToken = "${LUDUS_RUNTIME_HOST_OUT_DIR}";
		static constexpr std::string_view IntermediateDirectoryToken = "${LUDUS_RUNTIME_HOST_INT_DIR}";
		static constexpr std::string_view TargetNameToken = "${LUDUS_RUNTIME_HOST_TARGET_NAME}";
		static constexpr std::string_view EngineGladIncludeDirectoryToken = "${LUDUS_RUNTIME_HOST_ENGINE_GLAD_INCLUDE_DIR}";
		static constexpr std::string_view EngineIncludeDirectoryToken = "${LUDUS_RUNTIME_HOST_ENGINE_INCLUDE_DIR}";
		static constexpr std::string_view EngineVendorDirectoryToken = "${LUDUS_RUNTIME_HOST_ENGINE_VENDOR_DIR}";
		static constexpr std::string_view EngineResourcesDirectoryToken = "${LUDUS_RUNTIME_HOST_ENGINE_RESOURCES_DIR}";
		static constexpr std::string_view EngineProjectPathToken = "${LUDUS_RUNTIME_HOST_ENGINE_PROJECT_PATH}";
		static constexpr std::string_view RuntimeNameToken = "${LUDUS_RUNTIME_NAME}";
		static constexpr std::string_view ProjectGuidToken = "${LUDUS_RUNTIME_HOST_PROJECT_GUID}";

		static constexpr std::string_view BuildCommand = "Build";
		static constexpr std::string_view RebuildCommand = "Rebuild";
		static constexpr std::string_view CleanCommand = "Clean";
	};

	void RemoveProjectRuntimeHostBuildLayout(const std::filesystem::path& projectRoot)
	{
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::RuntimeHostBinDirectory(projectRoot));
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::RuntimeHostObjDirectory(projectRoot));

		Ludus::Engine::FileSystem::RemoveDirectoryIfEmpty(Ludus::Editor::Persistence::BuildPaths::BinDirectory(projectRoot));
		Ludus::Engine::FileSystem::RemoveDirectoryIfEmpty(Ludus::Editor::Persistence::BuildPaths::ObjDirectory(projectRoot));
	}

	void StageRuntimeHostManifest(
		const std::filesystem::path& manifestFrom,
		const std::filesystem::path& outputRoot,
		std::string_view runtimeName
	)
	{
		Ludus::Engine::Persistence::LmlRuntimeManifestPersistence runtimeManifestPersistence;
		auto runtimeManifest = runtimeManifestPersistence.Load(manifestFrom);

		Ludus::Editor::Build::RewriteScenePathsForPackagedRuntime(runtimeManifest);

		runtimeManifestPersistence.Save(
			runtimeManifest,
			Ludus::Engine::Persistence::Paths::RuntimeManifestFile(outputRoot, runtimeName)
		);
	}
}

namespace Ludus::Editor::Build::MSBuild
{
	void MSBuildRuntimeHostPipeline::CopyTemplateToDestinationIfMissing(
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
		auto text = Ludus::Engine::FileSystem::ReadAllText(sourcePath);
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::ProjectGuidToken, Ludus::Engine::Platform::CreateGuid().ToString());
		Ludus::Engine::FileSystem::WriteAllText(destinationPath, text);
	}

	void MSBuildRuntimeHostPipeline::EnsureBuildFiles(const std::filesystem::path& projectRoot)
	{
		Ludus::Editor::Persistence::BuildPaths::EnsureProjectRuntimeHostLayoutExists(projectRoot);

		const auto templateRoot = Ludus::Editor::Persistence::RepositoryPaths::RuntimeHostTemplatesDirectory();
		const auto runtimeHostObjDirectory = Ludus::Editor::Persistence::BuildPaths::RuntimeHostObjDirectory(projectRoot);

		CopyTemplateToDestinationIfMissing(
			templateRoot,
			Constants::RuntimeHostProjectTemplateFile,
			runtimeHostObjDirectory / std::string(Constants::RuntimeHostProjectFileName)
		);

		CopyTemplateToDestinationIfMissing(
			templateRoot,
			Constants::RuntimeHostModuleTemplateFile,
			runtimeHostObjDirectory / std::string(Constants::RuntimeHostModuleFileName)
		);
	}

	void MSBuildRuntimeHostPipeline::SetRuntimeHostProjectCompilationSettings(
		const std::filesystem::path& projectRoot,
		const RuntimeHostBuildSettings& settings
	)
	{
		const auto projectPath = Ludus::Editor::Persistence::BuildPaths::RuntimeHostProjectFile(projectRoot);
		auto text = Ludus::Engine::FileSystem::ReadAllText(projectPath);

		if (text.find("${") == std::string::npos)
		{
			return;
		}

		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::OutDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.OutDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::IntermediateDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.InDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::TargetNameToken, settings.TargetName);
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::EngineGladIncludeDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.EngineGladIncludeDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::EngineIncludeDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.EngineIncludeDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::EngineVendorDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.EngineVendorIncludeDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::EngineResourcesDirectoryToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.EngineResourcesIncludeDirectory));
		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::EngineProjectPathToken, Ludus::Engine::FileSystem::ToPortablePathString(settings.EngineProjectPath));

		if (text.find("${") != std::string::npos)
		{
			throw std::runtime_error("RuntimeHost.vcxproj still contains unresolved ${...} placeholders.");
		}

		Ludus::Engine::FileSystem::WriteAllText(projectPath, text);
	}

	void MSBuildRuntimeHostPipeline::SetRuntimeHostModuleManifestName(
		const std::filesystem::path& projectRoot,
		const RuntimeHostBuildSettings& settings
	)
	{
		const auto modulePath = Ludus::Editor::Persistence::BuildPaths::RuntimeHostModuleFile(projectRoot);
		auto text = Ludus::Engine::FileSystem::ReadAllText(modulePath);

		if (text.find("${") == std::string::npos)
		{
			return;
		}

		text = Ludus::Engine::Core::Strings::ReplaceAll(text, Constants::RuntimeNameToken, settings.RuntimeName);

		if (text.find("${") != std::string::npos)
		{
			throw std::runtime_error("RuntimeHostModule.cpp still contains unresolved ${...} placeholders.");
		}

		Ludus::Engine::FileSystem::WriteAllText(modulePath, text);
	}

	MSBuildRuntimeHostPipeline::MSBuildRuntimeHostPipeline(MSBuildContext& context)
		: m_Context(context)
	{ }

	void MSBuildRuntimeHostPipeline::RunBuild(
		const std::filesystem::path& projectRoot,
		BuildConfiguration configuration,
		BuildCommand command
	)
	{
		if (command == BuildCommand::Build || command == BuildCommand::Rebuild)
		{
			Ludus::Editor::Persistence::BuildPaths::EnsureProjectRuntimeHostBuildLayoutExists(projectRoot);
		}

		if (!m_Context.MSBuildPath)
		{
			throw std::runtime_error("MSBuild path not found.");
		}

		const auto projectPath = Ludus::Editor::Persistence::BuildPaths::RuntimeHostProjectFile(projectRoot);

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

		if (command == BuildCommand::Build || command == BuildCommand::Rebuild)
		{
			PopulateRuntimeHostOutput(projectRoot, configuration);
		}

		if (command == BuildCommand::Clean)
		{
			RemoveProjectRuntimeHostBuildLayout(projectRoot);
		}
	}

	void MSBuildRuntimeHostPipeline::PopulateRuntimeHostOutput(
		const std::filesystem::path& projectRoot,
		BuildConfiguration configuration
	)
	{
		const auto runtimeHostOutputDirectory = Ludus::Editor::Persistence::BuildPaths::RuntimeHostBinDirectory(
			projectRoot,
			BuildPlatform::WindowsX64,
			configuration
		);
		const auto projectName = projectRoot.filename().string();

		Ludus::Engine::FileSystem::ReplaceDirectory(
			Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot),
			Ludus::Engine::Persistence::Paths::AssetsDirectory(runtimeHostOutputDirectory)
		);
		Ludus::Engine::FileSystem::ReplaceDirectory(
			Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot),
			Ludus::Engine::Persistence::Paths::ScenesDirectory(runtimeHostOutputDirectory)
		);

		StageRuntimeHostManifest(
			Ludus::Engine::Persistence::Paths::RuntimeManifestFile(projectRoot, projectName),
			runtimeHostOutputDirectory,
			projectName
		);

		const auto scriptsModulePath = Ludus::Engine::Persistence::Paths::ScriptsDllFile(
			Ludus::Editor::Persistence::BuildPaths::ScriptsBinDirectory(
				projectRoot,
				BuildPlatform::WindowsX64,
				configuration
			)
		);
		if (std::filesystem::exists(scriptsModulePath))
		{
			Ludus::Engine::FileSystem::CopyFileOverwrite(
				scriptsModulePath,
				Ludus::Engine::Persistence::Paths::ScriptsDllFile(runtimeHostOutputDirectory)
			);
		}
		else
		{
			LUDUS_LOG_WARN("Skipping Scripts.dll staging for runtime host because the scripts build output does not exist.");
		}
	}

	void MSBuildRuntimeHostPipeline::EnsureRuntimeHostProject(
		const std::filesystem::path& projectRoot,
		const RuntimeHostBuildSettings& settings
	)
	{
		EnsureBuildFiles(projectRoot);
		SetRuntimeHostProjectCompilationSettings(projectRoot, settings);
		SetRuntimeHostModuleManifestName(projectRoot, settings);
	}
}
