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
		const auto expected = Ludus::Engine::Platform::Paths::LocalAppData() / "Ludus";
		const auto path = ProjectPaths::LudusRoot();
		ASSERT_EQ(path, expected);
	}

	TEST(ProjectPaths, ProjectsRoot_Should_ReturnProjectsDirectoryUnderLudusRoot)
	{
		const auto expected = ProjectPaths::LudusRoot() / "Projects";
		const auto path = ProjectPaths::ProjectsRoot();
		ASSERT_EQ(path, expected);
	}

	TEST(ProjectPaths, ProjectRoot_Should_ReturnProjectPathUnderProjectsRoot)
	{
		const auto path = ProjectPaths::ProjectRoot("Sandbox");
		ASSERT_EQ(path, ProjectPaths::ProjectsRoot() / "Sandbox");
	}

	TEST(ProjectPaths, ProjectManifestFile_Should_ReturnProjectManifestName)
	{
		const auto path = ProjectPaths::ProjectManifestFile("Sandbox");
		ASSERT_EQ(path, std::filesystem::path("Sandbox.ludus.project"));
	}

	TEST(ProjectPaths, ProjectManifestFile_WithRoot_Should_ReturnProjectManifestPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = ProjectPaths::ProjectManifestFile(projectRoot, "Sandbox");
		ASSERT_EQ(path, projectRoot / "Sandbox.ludus.project");
	}

	TEST(ProjectPaths, ValidateProjectName_Should_ReturnError_WhenNameIsEmpty)
	{
		const auto error = ProjectPaths::ValidateProjectName("");
		ASSERT_EQ(error, "Name must not be empty.");
	}

	TEST(ProjectPaths, ValidateProjectName_Should_ReturnError_WhenNameContainsInvalidCharacters)
	{
		const auto error = ProjectPaths::ValidateProjectName("Sand:box");
		ASSERT_EQ(error, "Name contains invalid path characters.");
	}

	TEST(ProjectPaths, ValidateScriptName_Should_ReturnError_WhenNameIsEmpty)
	{
		const auto error = ProjectPaths::ValidateScriptName("");
		ASSERT_EQ(error, "Name must not be empty.");
	}

	TEST(ProjectPaths, ValidateScriptName_Should_ReturnError_WhenNameContainsInvalidCharacters)
	{
		const auto error = ProjectPaths::ValidateScriptName("Player/Controller");
		ASSERT_EQ(error, "Name contains invalid path characters.");
	}

	TEST(ProjectPaths, ValidateScriptName_Should_ReturnEmpty_WhenNameIsValid)
	{
		const auto error = ProjectPaths::ValidateScriptName("PlayerController");
		ASSERT_TRUE(error.empty());
	}

	TEST(ProjectPaths, ValidateProjectDirectory_Should_ReturnError_WhenDirectoryIsEmpty)
	{
		const auto error = ProjectPaths::ValidateProjectDirectory({ });
		ASSERT_EQ(error, "Project directory is invalid.");
	}

	TEST(ProjectPaths, ValidateProjectDirectory_Should_ReturnError_WhenDirectoryAlreadyExists)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";
		std::filesystem::create_directories(projectRoot);

		const auto error = ProjectPaths::ValidateProjectDirectory(projectRoot);

		ASSERT_EQ(error, "Project directory already exists.");
	}

	TEST(ProjectPaths, ValidateProjectDirectory_Should_ReturnEmpty_WhenDirectoryIsNew)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		const auto error = ProjectPaths::ValidateProjectDirectory(projectRoot);

		ASSERT_TRUE(error.empty());
	}

	TEST(ProjectPaths, ScriptsDirectory_Should_ReturnScriptsPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = ProjectPaths::ScriptsDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "Scripts");
	}

	TEST(ProjectPaths, ScriptsSourceDirectory_Should_ReturnScriptsSourcePath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = ProjectPaths::ScriptsSourceDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source");
	}

	TEST(ProjectPaths, ScriptSourceFile_Should_ReturnScriptSourcePath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = ProjectPaths::ScriptSourceFile(projectRoot, "PlayerController");
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "PlayerController.cpp");
	}

	TEST(ProjectPaths, ScriptsModuleFile_Should_ReturnScriptsModulePath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = ProjectPaths::ScriptsModuleFile(projectRoot);
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "ScriptsModule.cpp");
	}

	TEST(ProjectPaths, ScriptsProjectFile_Should_ReturnScriptsProjectPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = ProjectPaths::ScriptsProjectFile(projectRoot);
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source" / "Scripts.vcxproj");
	}

	TEST(ProjectPaths, EnsureProjectRootExists_Should_CreateProjectRoot)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		ProjectPaths::EnsureProjectRootExists(projectRoot);

		ASSERT_TRUE(std::filesystem::exists(projectRoot));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot));
	}

	TEST(ProjectPaths, EnsureProjectScriptsSourceLayoutExists_Should_CreateScriptsSourceDirectory)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		ProjectPaths::EnsureProjectScriptsSourceLayoutExists(projectRoot);

		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scripts" / "Source"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "Scripts" / "Source"));
	}

	TEST(ProjectPaths, EnsureProjectLayoutExists_Should_CreateProjectRuntimeDirectories)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		ProjectPaths::EnsureProjectLayoutExists(projectRoot);

		ASSERT_TRUE(std::filesystem::exists(projectRoot));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot)));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot)));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scripts"));
	}
}
