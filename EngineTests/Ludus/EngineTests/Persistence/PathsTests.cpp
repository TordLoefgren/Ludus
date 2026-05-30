#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

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

	TEST(Paths, IsValidRuntimeAssetPath_Should_ReturnTrue_WhenPathIsUnderAssets)
	{
		// Arrange & Act.
		const auto isValid = Ludus::Engine::Persistence::Paths::IsValidRuntimeAssetPath("Assets/Player.png");

		// Assert.
		ASSERT_TRUE(isValid);
	}

	TEST(Paths, IsValidRuntimeAssetPath_Should_ReturnFalse_WhenPathIsOutsideAssets)
	{
		// Arrange & Act.
		const auto isValid = Ludus::Engine::Persistence::Paths::IsValidRuntimeAssetPath("Scenes/Main.scene.ludus");

		// Assert.
		ASSERT_FALSE(isValid);
	}

	TEST(Paths, IsValidRuntimeScenePath_Should_ReturnTrue_WhenPathIsSceneUnderScenes)
	{
		// Arrange & Act.
		const auto isValid = Ludus::Engine::Persistence::Paths::IsValidRuntimeScenePath("Scenes/Main.scene.ludus");

		// Assert.
		ASSERT_TRUE(isValid);
	}

	TEST(Paths, IsValidRuntimeScenePath_Should_ReturnFalse_WhenPathHasWrongExtension)
	{
		// Arrange & Act.
		const auto isValid = Ludus::Engine::Persistence::Paths::IsValidRuntimeScenePath("Scenes/Main.txt");

		// Assert.
		ASSERT_FALSE(isValid);
	}

	TEST(Paths, NormalizeRuntimeScenePathOrEmpty_Should_ReturnRelativeScenePath)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::NormalizeRuntimeScenePathOrEmpty(
			runtimeRootDirectory,
			runtimeRootDirectory / "Scenes" / "Main.scene.ludus"
		);

		// Assert.
		ASSERT_EQ(path, std::filesystem::path("Scenes") / "Main.scene.ludus");
	}

	TEST(Paths, NormalizeRuntimeAssetPathOrEmpty_Should_ReturnEmpty_WhenPathIsOutsideAssets)
	{
		// Arrange.
		const auto runtimeRootDirectory = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Engine::Persistence::Paths::NormalizeRuntimeAssetPathOrEmpty(
			runtimeRootDirectory,
			runtimeRootDirectory / "Scenes" / "Main.scene.ludus"
		);

		// Assert.
		ASSERT_TRUE(path.empty());
	}

	TEST(Paths, ValidateRuntimeManifestPaths_Should_Throw_WhenScenePathIsInvalid)
	{
		// Arrange.
		auto runtimeManifest = Ludus::Engine::Runtime::RuntimeManifest();
		runtimeManifest.Scenes.push_back({
			Ludus::Engine::Core::SceneId { 1 },
			"Main",
			"Assets/Main.scene.ludus"
			});

		// Act & Assert.
		ASSERT_THROW(
			Ludus::Engine::Persistence::Paths::ValidateRuntimeManifestPaths(runtimeManifest),
			std::runtime_error
		);
	}
}
