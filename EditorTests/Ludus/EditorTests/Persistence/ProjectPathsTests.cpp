#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::EditorTests::Persistence
{
	namespace FileSystem = Ludus::Engine::FileSystem;
	namespace ProjectPaths = Ludus::Editor::Persistence::ProjectPaths;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Editor_ProjectPaths_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	TEST(ProjectPaths, LudusRoot_Should_ReturnLocalAppDataLudusDirectory)
	{
		// Arrange.
		const auto expected = Ludus::Engine::Platform::Paths::LocalAppData() / "Ludus";

		// Act.
		const auto path = ProjectPaths::LudusRoot();

		// Assert.
		ASSERT_EQ(path, expected);
	}

	TEST(ProjectPaths, ProjectsRoot_Should_ReturnProjectsDirectoryUnderLudusRoot)
	{
		// Arrange.
		const auto expected = ProjectPaths::LudusRoot() / "Projects";

		// Act.
		const auto path = ProjectPaths::ProjectsRoot();

		// Assert.
		ASSERT_EQ(path, expected);
	}

	TEST(ProjectPaths, ProjectRoot_Should_ReturnProjectPathUnderProjectsRoot)
	{
		// Arrange & Act.
		const auto path = ProjectPaths::ProjectRoot("Sandbox");

		// Assert.
		ASSERT_EQ(path, ProjectPaths::ProjectsRoot() / "Sandbox");
	}

	TEST(ProjectPaths, ProjectManifestFile_Should_ReturnProjectManifestName)
	{
		// Arrange & Act.
		const auto path = ProjectPaths::ProjectManifestFile("Sandbox");

		// Assert.
		ASSERT_EQ(path, std::filesystem::path("Sandbox.project.ludus"));
	}

	TEST(ProjectPaths, ProjectManifestFile_WithRoot_Should_ReturnProjectManifestPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::ProjectManifestFile(projectRoot, "Sandbox");

		// Assert.
		ASSERT_EQ(path, projectRoot / "Sandbox.project.ludus");
	}

	TEST(ProjectPaths, ValidateFileName_Should_ReturnError_WhenNameIsEmpty)
	{
		// Arrange & Act.
		const auto error = ProjectPaths::ValidateFileName("");

		// Assert.
		ASSERT_EQ(error, "Name must not be empty.");
	}

	TEST(ProjectPaths, ValidateFileName_Should_ReturnError_WhenNameContainsInvalidCharacters)
	{
		// Arrange & Act.
		const auto error = ProjectPaths::ValidateFileName("Sand:box");

		// Assert.
		ASSERT_EQ(error, "Name contains invalid path characters.");
	}

	TEST(ProjectPaths, SceneFileName_Should_ReturnSceneFileName)
	{
		// Arrange & Act.
		const auto path = ProjectPaths::SceneFileName("Sandbox");

		// Assert.
		ASSERT_EQ(path, std::filesystem::path("Sandbox.scene.ludus"));
	}

	TEST(ProjectPaths, SceneFile_Should_ReturnProjectScenePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::SceneFile(projectRoot, "Main");

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scenes" / "Main.scene.ludus");
	}

	TEST(ProjectPaths, SceneFileInDirectory_Should_ReturnScenePathUnderDirectory)
	{
		// Arrange.
		const auto directory = std::filesystem::path("C:/Projects/Sandbox/Scenes");

		// Act.
		const auto path = ProjectPaths::SceneFileInDirectory(directory, "Main");

		// Assert.
		ASSERT_EQ(path, directory / "Main.scene.ludus");
	}

	TEST(ProjectPaths, SceneName_Should_ReturnSceneNameFromPath)
	{
		// Arrange & Act.
		const auto name = ProjectPaths::SceneName("C:/Projects/Sandbox/Scenes/Main.scene.ludus");

		// Assert.
		ASSERT_EQ(name, "Main");
	}

	TEST(ProjectPaths, ValidateAvailablePath_Should_ReturnError_WhenPathIsEmpty)
	{
		// Arrange & Act.
		const auto error = ProjectPaths::ValidateAvailablePath({ });

		// Assert.
		ASSERT_EQ(error, "Path is invalid.");
	}

	TEST(ProjectPaths, ValidateAvailablePath_Should_ReturnError_WhenPathAlreadyExists)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";
		std::filesystem::create_directories(projectRoot);

		// Act.
		const auto error = ProjectPaths::ValidateAvailablePath(projectRoot);

		// Assert.
		ASSERT_EQ(error, "Path already exists.");
	}

	TEST(ProjectPaths, ValidateAvailablePath_Should_ReturnEmpty_WhenPathIsNew)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		const auto error = ProjectPaths::ValidateAvailablePath(projectRoot);

		// Assert.
		ASSERT_TRUE(error.empty());
	}

	TEST(ProjectPaths, ScriptsDirectory_Should_ReturnScriptsPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::ScriptsDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts");
	}

	TEST(ProjectPaths, ScriptsSourceDirectory_Should_ReturnScriptsSourcePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::ScriptsSourceDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source");
	}

	TEST(ProjectPaths, ScriptSourceFile_Should_ReturnScriptSourcePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::ScriptSourceFile(projectRoot, "PlayerController");

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "PlayerController.cpp");
	}

	TEST(ProjectPaths, ScriptsModuleFile_Should_ReturnScriptsModulePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::ScriptsModuleFile(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "ScriptsModule.cpp");
	}

	TEST(ProjectPaths, ScriptsProjectFile_Should_ReturnScriptsProjectPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = ProjectPaths::ScriptsProjectFile(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "Scripts.vcxproj");
	}

	TEST(ProjectPaths, ScriptsSolutionFile_Should_ReturnScriptsSolutionPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Solutions/Sandbox");

		// Act.
		const auto path = ProjectPaths::ScriptsSolutionFile(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "Scripts.sln");
	}

	TEST(ProjectPaths, EnsureProjectRootExists_Should_CreateProjectRoot)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		ProjectPaths::EnsureProjectRootExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot));
	}

	TEST(ProjectPaths, EnsureProjectScriptsSourceLayoutExists_Should_CreateScriptsSourceDirectory)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		ProjectPaths::EnsureProjectScriptsSourceLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scripts" / "Source"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "Scripts" / "Source"));
	}

	TEST(ProjectPaths, EnsureProjectLayoutExists_Should_CreateProjectRuntimeDirectories)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		ProjectPaths::EnsureProjectLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot)));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot)));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scripts"));
	}
}
