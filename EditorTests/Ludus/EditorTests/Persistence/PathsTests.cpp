#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildOperatingSystem.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::EditorTests::Persistence
{
	namespace FileSystem = Ludus::Engine::FileSystem;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Editor_Paths_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	TEST(Paths, LudusRoot_Should_ReturnLocalAppDataLudusDirectory)
	{
		// Arrange.
		const auto expected = Ludus::Engine::Platform::Paths::LocalAppData() / "Ludus";

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::LudusRoot();

		// Assert.
		ASSERT_EQ(path, expected);
	}

	TEST(Paths, ProjectsRoot_Should_ReturnProjectsDirectoryUnderLudusRoot)
	{
		// Arrange.
		const auto expected = Ludus::Editor::Persistence::Paths::LudusRoot() / "Projects";

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ProjectsRoot();

		// Assert.
		ASSERT_EQ(path, expected);
	}

	TEST(Paths, ProjectRoot_Should_ReturnProjectPathUnderProjectsRoot)
	{
		// Arrange.
		const auto projectName = std::string_view("Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ProjectRoot(projectName);

		// Assert.
		ASSERT_EQ(path, Ludus::Editor::Persistence::Paths::ProjectsRoot() / "Sandbox");
	}

	TEST(Paths, ProjectManifestFile_Should_ReturnProjectManifestName)
	{
		// Arrange.
		const auto projectName = std::string_view("Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ProjectManifestFile(projectName);

		// Assert.
		ASSERT_EQ(path, std::filesystem::path("Sandbox.ludus.project"));
	}

	TEST(Paths, ProjectManifestFile_WithRoot_Should_ReturnProjectManifestPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto projectName = std::string_view("Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ProjectManifestFile(projectRoot, projectName);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Sandbox.ludus.project");
	}

	TEST(Paths, ValidateProjectName_Should_ReturnError_WhenNameIsEmpty)
	{
		const auto error = Ludus::Editor::Persistence::Paths::ValidateProjectName("");
		ASSERT_EQ(error, "Name must not be empty.");
	}

	TEST(Paths, ValidateProjectName_Should_ReturnError_WhenNameContainsInvalidCharacters)
	{
		const auto error = Ludus::Editor::Persistence::Paths::ValidateProjectName("Sand:box");
		ASSERT_EQ(error, "Name contains invalid path characters.");
	}

	TEST(Paths, ValidateScriptName_Should_ReturnError_WhenNameIsEmpty)
	{
		const auto error = Ludus::Editor::Persistence::Paths::ValidateScriptName("");
		ASSERT_EQ(error, "Name must not be empty.");
	}

	TEST(Paths, ValidateScriptName_Should_ReturnError_WhenNameContainsInvalidCharacters)
	{
		const auto error = Ludus::Editor::Persistence::Paths::ValidateScriptName("Player/Controller");
		ASSERT_EQ(error, "Name contains invalid path characters.");
	}

	TEST(Paths, ValidateScriptName_Should_ReturnEmpty_WhenNameIsValid)
	{
		const auto error = Ludus::Editor::Persistence::Paths::ValidateScriptName("PlayerController");
		ASSERT_TRUE(error.empty());
	}

	TEST(Paths, ScriptsSourceDirectory_Should_ReturnScriptsSourcePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ScriptsSourceDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Scripts" / "Source");
	}

	TEST(Paths, ScriptsBinDirectory_Should_ReturnScriptsBinPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ScriptsBinDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Bin" / "Scripts");
	}

	TEST(Paths, ScriptsBinDirectory_WithPlatformAndConfiguration_Should_ReturnScriptsBuildOutputPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ScriptsBinDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Bin" / "Scripts" / "x64" / "Release");
	}

	TEST(Paths, ScriptsObjDirectory_Should_ReturnScriptsObjPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ScriptsObjDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Obj" / "Scripts");
	}

	TEST(Paths, ScriptsObjDirectory_WithPlatformAndConfiguration_Should_ReturnScriptsIntermediatePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::ScriptsObjDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Debug
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Obj" / "Scripts" / "x64" / "Debug");
	}

	TEST(Paths, BuildsDirectory_Should_ReturnBuildsPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::BuildsDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Builds");
	}

	TEST(Paths, BuildOutputDirectory_Should_ReturnBuildOutputPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = Ludus::Editor::Persistence::Paths::BuildOutputDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildOperatingSystem::Windows,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "Builds" / "Windows" / "x64" / "Release");
	}

	TEST(Paths, EnsureProjectRootExists_Should_CreateProjectRoot)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		Ludus::Editor::Persistence::Paths::EnsureProjectRootExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot));
	}

	TEST(Paths, EnsureProjectScriptsSourceLayoutExists_Should_CreateScriptsSourceDirectory)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		Ludus::Editor::Persistence::Paths::EnsureProjectScriptsSourceLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scripts" / "Source"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "Scripts" / "Source"));
	}

	TEST(Paths, EnsureProjectScriptsBuildLayoutExists_Should_CreateScriptsBuildDirectories)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		Ludus::Editor::Persistence::Paths::EnsureProjectScriptsBuildLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Bin" / "Scripts"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "Bin" / "Scripts"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Obj" / "Scripts"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "Obj" / "Scripts"));
	}

	TEST(Paths, EnsureProjectLayoutExists_Should_CreateProjectRuntimeDirectories)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		Ludus::Editor::Persistence::Paths::EnsureProjectLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Assets"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scenes"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "Scripts"));
	}
}
