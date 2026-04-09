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
		ASSERT_EQ(path, std::filesystem::path("Sandbox.runtime.ludus"));
	}

	TEST(Paths, RuntimeManifestFile_WithRoot_Should_ReturnPathUnderRuntimeRoot)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");
		const auto runtimeName = std::string_view("Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::RuntimeManifestFile(runtimeRootDirectory, runtimeName);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Sandbox.runtime.ludus");
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
		ASSERT_EQ(path, runtimeRootDirectory / "Scenes" / "MainMenu.scene.ludus");
	}

	TEST(Paths, RuntimeRelativeSceneFile_Should_ReturnScenePathRelativeToRuntimeRoot)
	{
		// Arrange.
		const auto scenePath = std::filesystem::path("C:/Projects/Sandbox/Scenes/MainMenu.scene.ludus");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::RuntimeRelativeSceneFile(scenePath);

		// Assert.
		ASSERT_EQ(path, std::filesystem::path("Scenes") / "MainMenu.scene.ludus");
	}

	TEST(Paths, ShadersDirectory_Should_ReturnShadersPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::ShadersDirectory(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Resources" / "Shaders");
	}

	TEST(Paths, DefaultFontFile_Should_ReturnDefaultFontPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::DefaultFontFile(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Resources" / "Fonts" / "liberation-sans" / "LiberationSans-Regular.ttf");
	}

	TEST(Paths, ScriptsDllFile_Should_ReturnScriptsDllPath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::ScriptsDllFile(runtimeRootDirectory);

		// Assert.
		ASSERT_EQ(path, runtimeRootDirectory / "Scripts.dll");
	}
}
