#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildOperatingSystem.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Persistence/BuildPaths.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/Paths.h>

namespace Ludus::EditorTests::Persistence
{
	namespace BuildPaths = Ludus::Editor::Persistence::BuildPaths;
	namespace FileSystem = Ludus::Engine::FileSystem;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Editor_BuildPaths_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	TEST(BuildPaths, BinDirectory_Should_ReturnBinPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::BinDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "bin");
	}

	TEST(BuildPaths, ObjDirectory_Should_ReturnObjPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::ObjDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj");
	}

	TEST(BuildPaths, BuildsDirectory_Should_ReturnBuildsPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::BuildsDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "builds");
	}

	TEST(BuildPaths, RuntimeOutputDirectory_Should_ReturnRuntimeOutputPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeOutputDirectory(
			projectRoot,
			"Sandbox",
			Ludus::Editor::Build::BuildOperatingSystem::Windows,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "builds" / "Windows" / "x64" / "Release" / "Sandbox");
	}

	TEST(BuildPaths, ScriptsBinDirectory_Should_ReturnScriptsBinPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::ScriptsBinDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "bin" / "Scripts");
	}

	TEST(BuildPaths, ScriptsBinDirectory_WithPlatformAndConfiguration_Should_ReturnScriptsBuildOutputPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::ScriptsBinDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "bin" / "Scripts" / "x64" / "Release");
	}

	TEST(BuildPaths, ScriptsObjDirectory_Should_ReturnScriptsObjPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::ScriptsObjDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj" / "Scripts");
	}

	TEST(BuildPaths, ScriptsObjDirectory_WithPlatformAndConfiguration_Should_ReturnScriptsIntermediatePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::ScriptsObjDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Debug
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj" / "Scripts" / "x64" / "Debug");
	}

	TEST(BuildPaths, ScriptsDllFile_Should_ReturnScriptsDllPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::ScriptsDllFile(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "bin" / "Scripts" / "x64" / "Release" / "Scripts.dll");
	}

	TEST(BuildPaths, RuntimeHostBinDirectory_Should_ReturnRuntimeHostBinPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeHostBinDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "bin" / "RuntimeHost");
	}

	TEST(BuildPaths, RuntimeHostBinDirectory_WithPlatformAndConfiguration_Should_ReturnRuntimeHostBuildOutputPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeHostBinDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Debug
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "bin" / "RuntimeHost" / "x64" / "Debug");
	}

	TEST(BuildPaths, RuntimeHostExecutableFile_Should_ReturnRuntimeHostExecutablePath)
	{
		// Arrange.
		const auto runtimeHostRoot = std::filesystem::path("C:/Projects/Sandbox/bin/RuntimeHost/x64/Debug");

		// Act.
		const auto path = BuildPaths::RuntimeHostExecutableFile(runtimeHostRoot);

		// Assert.
		ASSERT_EQ(path, runtimeHostRoot / "RuntimeHost.exe");
	}

	TEST(BuildPaths, RuntimeExecutableFile_Should_ReturnRuntimeExecutablePath)
	{
		// Arrange.
		const auto runtimeOutputRoot = std::filesystem::path("C:/Projects/Sandbox/builds/Windows/x64/Release/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeExecutableFile(runtimeOutputRoot, "Sandbox");

		// Assert.
		ASSERT_EQ(path, runtimeOutputRoot / "Sandbox.exe");
	}

	TEST(BuildPaths, RuntimeHostObjDirectory_Should_ReturnRuntimeHostObjPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeHostObjDirectory(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost");
	}

	TEST(BuildPaths, RuntimeHostObjDirectory_WithPlatformAndConfiguration_Should_ReturnRuntimeHostIntermediatePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeHostObjDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost" / "x64" / "Release");
	}

	TEST(BuildPaths, RuntimeHostModuleFile_Should_ReturnRuntimeHostModulePath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeHostModuleFile(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost" / "RuntimeHostModule.cpp");
	}

	TEST(BuildPaths, RuntimeHostProjectFile_Should_ReturnRuntimeHostProjectPath)
	{
		// Arrange.
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = BuildPaths::RuntimeHostProjectFile(projectRoot);

		// Assert.
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost" / "RuntimeHost.vcxproj");
	}

	TEST(BuildPaths, EnsureRuntimeOutputDirectory_Should_CreateRuntimeOutputDirectories)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		BuildPaths::EnsureRuntimeOutputDirectory(
			projectRoot,
			"Sandbox",
			Ludus::Editor::Build::BuildOperatingSystem::Windows,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		// Assert.
		const auto runtimeOutputDirectory = projectRoot / "builds" / "Windows" / "x64" / "Release" / "Sandbox";
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::AssetsDirectory(runtimeOutputDirectory)));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::ResourcesDirectory(runtimeOutputDirectory)));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::ScenesDirectory(runtimeOutputDirectory)));
	}

	TEST(BuildPaths, EnsureProjectScriptsBuildLayoutExists_Should_CreateScriptsBuildDirectories)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		BuildPaths::EnsureProjectScriptsBuildLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "bin" / "Scripts"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "bin" / "Scripts"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "obj" / "Scripts"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "obj" / "Scripts"));
	}

	TEST(BuildPaths, EnsureProjectRuntimeHostBuildLayoutExists_Should_CreateRuntimeHostBuildDirectories)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		// Act.
		BuildPaths::EnsureProjectRuntimeHostBuildLayoutExists(projectRoot);

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "bin" / "RuntimeHost"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "bin" / "RuntimeHost"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "obj" / "RuntimeHost"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "obj" / "RuntimeHost"));
	}
}
