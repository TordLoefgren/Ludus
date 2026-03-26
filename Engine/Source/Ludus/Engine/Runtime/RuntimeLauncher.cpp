#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Engine/Persistence/LmlRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Paths.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeLauncher.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>

namespace
{
	Ludus::Engine::Runtime::RuntimeEnvironment BuildRuntimeEnvironment(std::string_view runtimeName)
	{
		const auto runtimeRootDirectory = Ludus::Engine::Platform::Paths::GetExecutablePath().parent_path();

		return Ludus::Engine::Runtime::RuntimeEnvironment
		{
			.RuntimeRootDirectory = runtimeRootDirectory,
			.ResourcesDirectory = Ludus::Engine::Persistence::Paths::ResourcesDirectory(runtimeRootDirectory),
			.ShadersDirectory = Ludus::Engine::Persistence::Paths::ShadersDirectory(runtimeRootDirectory),
			.DefaultFontPath = Ludus::Engine::Persistence::Paths::DefaultFontFile(runtimeRootDirectory),
			.RuntimeManifestPath = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(runtimeRootDirectory, runtimeName),
			.ScriptModulePath = Ludus::Engine::Persistence::Paths::ScriptsDllFile(runtimeRootDirectory)
		};
	}

	const Ludus::Engine::Runtime::SceneReference& FindEntrySceneReference(
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest
	)
	{
		for (const auto& scene : runtimeManifest.Scenes)
		{
			if (scene.Handle == runtimeManifest.EntrySceneHandle)
			{
				return scene;
			}
		}

		throw std::runtime_error("Runtime manifest entry scene was not found.");
	}
}

namespace Ludus::Engine::Runtime
{
	int RunDefaultRuntime(std::string_view runtimeName)
	{
		auto runtimeEnvironment = BuildRuntimeEnvironment(runtimeName);

		Ludus::Engine::Persistence::LmlRuntimeManifestPersistence runtimeManifestPersistence;
		const auto runtimeManifest = runtimeManifestPersistence.Load(runtimeEnvironment.RuntimeManifestPath);

		const auto& entrySceneReference = FindEntrySceneReference(runtimeManifest);
		const auto entryScenePath = Ludus::Engine::Persistence::Paths::ResolveRuntimeScenePath(
			runtimeEnvironment.RuntimeRootDirectory,
			entrySceneReference.Path
		);

		Ludus::Engine::Persistence::LmlScenePersistence scenePersistence;
		auto entryScene = scenePersistence.Load(entryScenePath);

		Ludus::Engine::Windowing::WindowOptions windowOptions;
		windowOptions.Title = runtimeName;

		auto host = Ludus::Engine::Runtime::ApplicationHostBuilder::Create()
			.WithWindowOptions(windowOptions)
			.Build();

		auto runtime = Ludus::Engine::Runtime::RuntimeInstanceBuilder::Create()
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D()
			.UseDefaultScripting()
			.UseDefaultMainRenderView()
			.WithRuntimeEnvironment(std::move(runtimeEnvironment))
			.WithRuntimeManifest(runtimeManifest)
			.WithEntryScene(std::move(entryScene))
			.Build(*host);

		host->AttachRuntime(runtime.get());
		host->Run();

		return 0;
	}
}
