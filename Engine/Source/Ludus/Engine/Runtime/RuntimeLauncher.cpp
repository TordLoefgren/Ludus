#include "pch.h"

#include <filesystem>
#include <stdexcept>
#include <utility>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/EnginePersistence.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Paths.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
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
			.RuntimeLaunchSettingsPath = Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(runtimeRootDirectory, runtimeName),
			.ScriptModulePath = Ludus::Engine::Persistence::Paths::ScriptsDllFile(runtimeRootDirectory)
		};
	}

	Ludus::Engine::Core::Scene LoadEntryScene(
		const Ludus::Engine::Persistence::IScenePersistence& scenePersistence,
		const Ludus::Engine::Runtime::RuntimeEnvironment& runtimeEnvironment,
		const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest
	)
	{
		const Ludus::Engine::Runtime::SceneReference* entrySceneReference = nullptr;
		if (runtimeManifest.EntrySceneId.IsValid())
		{
			for (const auto& sceneReference : runtimeManifest.Scenes)
			{
				if (sceneReference.Id == runtimeManifest.EntrySceneId)
				{
					entrySceneReference = &sceneReference;
					break;
				}
			}
		}

		if (!entrySceneReference)
		{
			throw std::runtime_error("Runtime manifest entry scene was not found.");
		}

		const auto scenePath = Ludus::Engine::Persistence::Paths::ResolveRuntimeScenePath(
			runtimeEnvironment.RuntimeRootDirectory,
			entrySceneReference->Path
		);

		auto scene = scenePersistence.Load(scenePath);

		std::vector<Ludus::Engine::Core::EntityId> unresolvedScriptOwners;
		for (const auto& script : scene.EntityComponentSystem.Scripts.View())
		{
			if (!runtimeManifest.TryGetScriptReference(script.Id))
			{
				LUDUS_LOG_WARN("Scene contains script id that is not present in the runtime manifest. The script component will be ignored at runtime.");
				unresolvedScriptOwners.push_back(script.OwnerId);
			}
		}

		for (const auto ownerId : unresolvedScriptOwners)
		{
			scene.EntityComponentSystem.Scripts.RemoveByOwner(ownerId);
		}

		return scene;
	}

	Ludus::Engine::Windowing::WindowOptions ToWindowOptions(
		const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings,
		std::string_view runtimeName
	)
	{
		Ludus::Engine::Windowing::WindowOptions windowOptions;
		windowOptions.Title = runtimeName;
		windowOptions.StartupWidth = runtimeLaunchSettings.WindowSettings.StartupWidth;
		windowOptions.StartupHeight = runtimeLaunchSettings.WindowSettings.StartupHeight;
		windowOptions.IsResizeable = runtimeLaunchSettings.WindowSettings.IsResizeable;
		windowOptions.IsMaximized = runtimeLaunchSettings.WindowSettings.IsMaximized;

		return windowOptions;
	}

	Ludus::Engine::Graphics::RenderPresentationSettings ToRenderPresentationSettings(const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings)
	{
		Ludus::Engine::Graphics::RenderPresentationSettings renderPresentationSettings;
		renderPresentationSettings.InternalResolution = runtimeLaunchSettings.PresentationSettings.InternalResolution;
		renderPresentationSettings.UseFixedRenderResolution = runtimeLaunchSettings.PresentationSettings.UseFixedRenderResolution;

		return renderPresentationSettings;
	}
}

namespace Ludus::Engine::Runtime
{
	int RuntimeLauncher::Run(std::string_view runtimeName)
	{
		auto runtimeEnvironment = BuildRuntimeEnvironment(runtimeName);
		auto enginePersistence = Ludus::Engine::Persistence::EnginePersistence::DefaultText();

		const auto runtimeManifest = enginePersistence
			.RuntimeManifest()
			.Load(runtimeEnvironment.RuntimeManifestPath);

		auto entryScene = ::LoadEntryScene(
			enginePersistence.Scene(),
			runtimeEnvironment,
			runtimeManifest
		);

		const auto runtimeLaunchSettings = enginePersistence
			.RuntimeLaunchSettings()
			.Load(runtimeEnvironment.RuntimeLaunchSettingsPath);

		auto host = Ludus::Engine::Runtime::ApplicationHostBuilder::Create()
			.WithEnginePersistence(std::move(enginePersistence))
			.WithWindowOptions(ToWindowOptions(runtimeLaunchSettings, runtimeName))
			.Build();

		auto runtime = Ludus::Engine::Runtime::RuntimeInstanceBuilder::Create()
			.UseDefaultPhysics2D()
			.UseDefaultRendering2D()
			.UseDefaultSceneManagement()
			.UseDefaultScripting()
			.UseDefaultMainRenderView()
			.WithRenderPresentationSettings(ToRenderPresentationSettings(runtimeLaunchSettings))
			.WithRuntimeEnvironment(std::move(runtimeEnvironment))
			.WithRuntimeManifest(runtimeManifest)
			.WithEntryScene(std::move(entryScene))
			.Build(*host);

		host->AttachRuntime(runtime.get());
		host->Run();

		return 0;
	}
}
