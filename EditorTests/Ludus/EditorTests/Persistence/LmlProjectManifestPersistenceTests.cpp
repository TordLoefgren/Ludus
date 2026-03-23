#include "pch.h"

#include <filesystem>
#include <stdexcept>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Editor/Persistence/LmlProjectManifestPersistence.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>

namespace Ludus::EditorTests::Persistence
{
	using ProjectManifest = Ludus::Editor::Core::ProjectManifest;
	using ProjectManifestPersistence = Ludus::Editor::Persistence::IProjectManifestPersistence;
	using LmlProjectManifestPersistence = Ludus::Editor::Persistence::LmlProjectManifestPersistence;
	namespace FileSystem = Ludus::Engine::FileSystem;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Editor_Persistence_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	TEST(LmlProjectManifestPersistence, SaveAndLoad_PreservesProjectManifest_When_ProjectManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto manifestPath = tempDirectoryScoped.Path / "Game.ludus.project";

		auto manifest = ProjectManifest::Create(
			std::filesystem::path("Projects/Game"),
			std::filesystem::path("Game.ludus.runtime")
		);

		LmlProjectManifestPersistence implementation;
		ProjectManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(manifest, manifestPath);
		const auto loaded = persistence.Load(manifestPath);

		// Assert.
		ASSERT_EQ(loaded.ProjectRoot, manifest.ProjectRoot);
		ASSERT_EQ(loaded.RuntimeManifestPath, manifest.RuntimeManifestPath);
	}

	TEST(LmlProjectManifestPersistence, SaveAndLoad_PreservesRuntimeManifestPath_When_ProjectManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto manifestPath = tempDirectoryScoped.Path / "Game.ludus.project";

		auto manifest = ProjectManifest::Create(
			std::filesystem::path("Projects/Game"),
			std::filesystem::path("Nested/Game.ludus.runtime")
		);

		LmlProjectManifestPersistence implementation;
		ProjectManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(manifest, manifestPath);
		const auto loaded = persistence.Load(manifestPath);

		// Assert.
		ASSERT_EQ(loaded.ProjectRoot, manifest.ProjectRoot);
		ASSERT_EQ(loaded.RuntimeManifestPath, manifest.RuntimeManifestPath);
	}

	TEST(LmlProjectManifestPersistence, Load_ThrowsRuntimeError_When_ProjectManifestIsMalformed)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto manifestPath = tempDirectoryScoped.Path / "Game.ludus.project";

		FileSystem::WriteAllText(manifestPath, "Version: { Major: 0, Minor: 2, Patch: 0 }");

		LmlProjectManifestPersistence implementation;
		ProjectManifestPersistence& persistence = implementation;

		// Act & Assert.
		ASSERT_THROW(persistence.Load(manifestPath), std::runtime_error);
	}

	TEST(LmlProjectManifestPersistence, SaveAndLoad_PreservesMinimalManifest_When_ProjectManifestIsMinimal)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto manifestPath = tempDirectoryScoped.Path / "Game.ludus.project";

		auto manifest = ProjectManifest::Create(std::filesystem::path(), std::filesystem::path());

		LmlProjectManifestPersistence implementation;
		ProjectManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(manifest, manifestPath);
		const auto loaded = persistence.Load(manifestPath);

		// Assert.
		ASSERT_EQ(loaded.ProjectRoot, manifest.ProjectRoot);
		ASSERT_EQ(loaded.RuntimeManifestPath, manifest.RuntimeManifestPath);
	}
}
