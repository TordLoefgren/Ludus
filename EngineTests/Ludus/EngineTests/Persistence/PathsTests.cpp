#include "pch.h"

#include <filesystem>

#include <Ludus/Engine/Core/Build/Configuration.h>
#include <Ludus/Engine/Core/Build/OperatingSystem.h>
#include <Ludus/Engine/Core/Build/Platform.h>
#include <Ludus/Engine/Persistence/Paths.h>

namespace Ludus::EngineTests::Persistence
{
	TEST(Paths, RuntimeManifestFile_Should_ReturnRuntimeManifestName)
	{
		// Arrange.
		const auto runtimeName = std::string_view("Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(runtimeName);

		// Assert.
		ASSERT_EQ(path, std::filesystem::path("Sandbox.ludus.runtime"));
	}

	TEST(Paths, RuntimeManifestFile_WithRoot_Should_ReturnPathUnderRuntimeRoot)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");
		const auto runtimeName = std::string_view("Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(runtimeRootDirectory, runtimeName);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Sandbox.ludus.runtime");
	}

	TEST(Paths, AssetsDirectory_Should_ReturnAssetsPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::AssetsDirectory(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Assets");
	}

	TEST(Paths, ScenesDirectory_Should_ReturnScenesPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::ScenesDirectory(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Scenes");
	}

	TEST(Paths, SceneFile_Should_ReturnScenePath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");
		const auto sceneName = std::string_view("MainMenu");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::SceneFile(runtimeRootDirectory, sceneName);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Scenes" / "MainMenu.ludus.scene");
	}

	TEST(Paths, ScriptsDllFile_Should_ReturnScriptsDllPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::ScriptsDllFile(
			runtimeRootDirectory,
			Ludus::Engine::Core::Build::Platform::WindowsX64,
			Ludus::Engine::Core::Build::Configuration::Debug
		);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Bin" / "Scripts" / "x64" / "Debug" / "Scripts.dll");
	}

	TEST(Paths, BuildOutputDirectory_Should_ReturnBuildOutputPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::BuildOutputDirectory(
			runtimeRootDirectory,
			Ludus::Engine::Core::Build::OperatingSystem::Windows,
			Ludus::Engine::Core::Build::Platform::WindowsX64,
			Ludus::Engine::Core::Build::Configuration::Release
		);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Builds" / "Windows" / "x64" / "Release");
	}
}
