#include "pch.h"

#include <filesystem>
#include <memory>
#include <string_view>
#include <utility>

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Build/IBuildPipeline.h>
#include <Ludus/Editor/Build/IPackagePipeline.h>
#include <Ludus/Editor/Build/MSBuild/MSBuildPipeline.h>
#include <Ludus/Editor/Build/RuntimePackage/RuntimeHostPackagePipeline.h>
#include <Ludus/Editor/Persistence/BuildPaths.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>

namespace Ludus::Editor::Build
{
	BuildManager::BuildManager(
		std::unique_ptr<IBuildPipeline> buildPipeline,
		std::unique_ptr<IPackagePipeline> packagePipeline
	)
	{
		m_BuildPipeline = std::move(buildPipeline);
		m_PackagePipeline = std::move(packagePipeline);
	}

	BuildManager::BuildManager()
		: BuildManager(
			std::make_unique<Ludus::Editor::Build::MSBuild::MSBuildPipeline>(),
			std::make_unique<Ludus::Editor::Build::RuntimePackage::RuntimeHostPackagePipeline>()
		)
	{ }

	void BuildManager::Initialize()
	{
		if (!m_BuildPipeline)
		{
			throw std::runtime_error("The build pipeline has not been initialized.");
		}

		m_BuildPipeline->Initialize();
		m_PackagePipeline->Initialize();
	}

	void BuildManager::BuildRuntime(
		const std::filesystem::path& projectRoot,
		BuildConfiguration configuration
	)
	{
		RunTargetBuildCommand(projectRoot, BuildTarget::Scripts, BuildCommand::Rebuild, configuration);
		RunTargetBuildCommand(projectRoot, BuildTarget::RuntimeHost, BuildCommand::Rebuild, configuration);
		m_PackagePipeline->CleanPackage(projectRoot, configuration, BuildPlatform::WindowsX64);
		m_PackagePipeline->BuildPackage(projectRoot, configuration, BuildPlatform::WindowsX64);
	}

	void BuildManager::CleanRuntime(
		const std::filesystem::path& projectRoot
	)
	{
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::BinDirectory(projectRoot));
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::ObjDirectory(projectRoot));
		Ludus::Engine::FileSystem::RemoveIfExists(Ludus::Editor::Persistence::BuildPaths::BuildsDirectory(projectRoot));
	}

	void BuildManager::EnsureRuntimeHostProject(
		const std::filesystem::path& projectRoot
	)
	{
		m_BuildPipeline->EnsureProject(projectRoot, BuildTarget::RuntimeHost);
	}

	void BuildManager::EnsureScriptProject(
		const std::filesystem::path& projectRoot
	)
	{
		m_BuildPipeline->EnsureProject(projectRoot, BuildTarget::Scripts);
	}

	void BuildManager::RunTargetBuildCommand(
		const std::filesystem::path& projectRoot,
		BuildTarget target,
		BuildCommand command,
		BuildConfiguration configuration
	)
	{
		switch (target)
		{
			case BuildTarget::RuntimeHost:
				EnsureRuntimeHostProject(projectRoot);
				break;

			case BuildTarget::Scripts:
				EnsureScriptProject(projectRoot);
				break;

			default:
				throw std::runtime_error("Unsupported build target.");
		}

		m_BuildPipeline->RunBuild(projectRoot, target, configuration, command);
	}

	void BuildManager::CreateScript(const std::filesystem::path& projectRoot, std::string_view name)
	{
		EnsureScriptProject(projectRoot);
		m_BuildPipeline->CreateScript(projectRoot, name);
	}
}
