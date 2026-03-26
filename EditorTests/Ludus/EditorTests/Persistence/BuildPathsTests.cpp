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
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::BinDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "bin");
	}

	TEST(BuildPaths, ObjDirectory_Should_ReturnObjPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::ObjDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "obj");
	}

	TEST(BuildPaths, BuildsDirectory_Should_ReturnBuildsPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::BuildsDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "builds");
	}

	TEST(BuildPaths, RuntimeOutputDirectory_Should_ReturnRuntimeOutputPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		const auto path = BuildPaths::RuntimeOutputDirectory(
			projectRoot,
			"Sandbox",
			Ludus::Editor::Build::BuildOperatingSystem::Windows,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		ASSERT_EQ(path, projectRoot / "builds" / "Windows" / "x64" / "Release" / "Sandbox");
	}

	TEST(BuildPaths, ScriptsBinDirectory_Should_ReturnScriptsBinPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::ScriptsBinDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "bin" / "Scripts");
	}

	TEST(BuildPaths, ScriptsBinDirectory_WithPlatformAndConfiguration_Should_ReturnScriptsBuildOutputPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		const auto path = BuildPaths::ScriptsBinDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		ASSERT_EQ(path, projectRoot / "bin" / "Scripts" / "x64" / "Release");
	}

	TEST(BuildPaths, ScriptsObjDirectory_Should_ReturnScriptsObjPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::ScriptsObjDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "obj" / "Scripts");
	}

	TEST(BuildPaths, ScriptsObjDirectory_WithPlatformAndConfiguration_Should_ReturnScriptsIntermediatePath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		const auto path = BuildPaths::ScriptsObjDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Debug
		);

		ASSERT_EQ(path, projectRoot / "obj" / "Scripts" / "x64" / "Debug");
	}

	TEST(BuildPaths, ScriptsDllFile_Should_ReturnScriptsDllPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		const auto path = BuildPaths::ScriptsDllFile(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		ASSERT_EQ(path, projectRoot / "bin" / "Scripts" / "x64" / "Release" / "Scripts.dll");
	}

	TEST(BuildPaths, RuntimeHostBinDirectory_Should_ReturnRuntimeHostBinPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::RuntimeHostBinDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "bin" / "RuntimeHost");
	}

	TEST(BuildPaths, RuntimeHostBinDirectory_WithPlatformAndConfiguration_Should_ReturnRuntimeHostBuildOutputPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		const auto path = BuildPaths::RuntimeHostBinDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Debug
		);

		ASSERT_EQ(path, projectRoot / "bin" / "RuntimeHost" / "x64" / "Debug");
	}

	TEST(BuildPaths, RuntimeHostExecutableFile_Should_ReturnRuntimeHostExecutablePath)
	{
		const auto runtimeHostRoot = std::filesystem::path("C:/Projects/Sandbox/bin/RuntimeHost/x64/Debug");
		const auto path = BuildPaths::RuntimeHostExecutableFile(runtimeHostRoot);
		ASSERT_EQ(path, runtimeHostRoot / "RuntimeHost.exe");
	}

	TEST(BuildPaths, RuntimeExecutableFile_Should_ReturnRuntimeExecutablePath)
	{
		const auto runtimeOutputRoot = std::filesystem::path("C:/Projects/Sandbox/builds/Windows/x64/Release/Sandbox");
		const auto path = BuildPaths::RuntimeExecutableFile(runtimeOutputRoot, "Sandbox");
		ASSERT_EQ(path, runtimeOutputRoot / "Sandbox.exe");
	}

	TEST(BuildPaths, RuntimeHostObjDirectory_Should_ReturnRuntimeHostObjPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::RuntimeHostObjDirectory(projectRoot);
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost");
	}

	TEST(BuildPaths, RuntimeHostObjDirectory_WithPlatformAndConfiguration_Should_ReturnRuntimeHostIntermediatePath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");

		const auto path = BuildPaths::RuntimeHostObjDirectory(
			projectRoot,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost" / "x64" / "Release");
	}

	TEST(BuildPaths, RuntimeHostModuleFile_Should_ReturnRuntimeHostModulePath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::RuntimeHostModuleFile(projectRoot);
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost" / "RuntimeHostModule.cpp");
	}

	TEST(BuildPaths, RuntimeHostProjectFile_Should_ReturnRuntimeHostProjectPath)
	{
		const auto projectRoot = std::filesystem::path("C:/Projects/Sandbox");
		const auto path = BuildPaths::RuntimeHostProjectFile(projectRoot);
		ASSERT_EQ(path, projectRoot / "obj" / "RuntimeHost" / "RuntimeHost.vcxproj");
	}

	TEST(BuildPaths, EnsureRuntimeOutputDirectory_Should_CreateRuntimeOutputDirectories)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		BuildPaths::EnsureRuntimeOutputDirectory(
			projectRoot,
			"Sandbox",
			Ludus::Editor::Build::BuildOperatingSystem::Windows,
			Ludus::Editor::Build::BuildPlatform::WindowsX64,
			Ludus::Editor::Build::BuildConfiguration::Release
		);

		const auto runtimeOutputDirectory = projectRoot / "builds" / "Windows" / "x64" / "Release" / "Sandbox";
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::AssetsDirectory(runtimeOutputDirectory)));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::ResourcesDirectory(runtimeOutputDirectory)));
		ASSERT_TRUE(std::filesystem::exists(Ludus::Engine::Persistence::Paths::ScenesDirectory(runtimeOutputDirectory)));
	}

	TEST(BuildPaths, EnsureProjectScriptsBuildLayoutExists_Should_CreateScriptsBuildDirectories)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		BuildPaths::EnsureProjectScriptsBuildLayoutExists(projectRoot);

		ASSERT_TRUE(std::filesystem::exists(projectRoot / "bin" / "Scripts"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "bin" / "Scripts"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "obj" / "Scripts"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "obj" / "Scripts"));
	}

	TEST(BuildPaths, EnsureProjectRuntimeHostBuildLayoutExists_Should_CreateRuntimeHostBuildDirectories)
	{
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";

		BuildPaths::EnsureProjectRuntimeHostBuildLayoutExists(projectRoot);

		ASSERT_TRUE(std::filesystem::exists(projectRoot / "bin" / "RuntimeHost"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "bin" / "RuntimeHost"));
		ASSERT_TRUE(std::filesystem::exists(projectRoot / "obj" / "RuntimeHost"));
		ASSERT_TRUE(std::filesystem::is_directory(projectRoot / "obj" / "RuntimeHost"));
	}
}
