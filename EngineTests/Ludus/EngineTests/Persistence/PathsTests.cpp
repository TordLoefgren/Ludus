#include "pch.h"

#include <filesystem>

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

	TEST(Paths, ScriptsDirectory_Should_ReturnScriptsPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::ScriptsDirectory(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Scripts");
	}

	TEST(Paths, ScriptsDllFile_Should_ReturnScriptsDllPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::ScriptsDllFile(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Scripts" / "Scripts.dll");
	}
}
