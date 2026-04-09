#include "pch.h"

#include <filesystem>
#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/LmlRuntimeManifestPersistence.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::EngineTests::Persistence
{
	using LmlRuntimeManifestPersistence = Ludus::Engine::Persistence::LmlRuntimeManifestPersistence;
	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using RuntimeManifestPersistence = Ludus::Engine::Persistence::IRuntimeManifestPersistence;
	using SceneReference = Ludus::Engine::Runtime::SceneReference;
	using ScriptReference = Ludus::Engine::Runtime::ScriptReference;
	using SceneId = Ludus::Engine::Core::SceneId;
	using ScriptId = Ludus::Engine::Core::ScriptId;
	namespace FileSystem = Ludus::Engine::FileSystem;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Engine_Persistence_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	static SceneReference MakeSceneReference(
		SceneId id,
		std::string_view name,
		std::string_view path
	)
	{
		return {
			.Id = id,
			.Name = std::string(name),
			.Path = std::filesystem::path(path)
		};
	}

	static ScriptReference MakeScriptReference(ScriptId id, std::string_view name)
	{
		return {
			.Id = id,
			.Name = std::string(name)
		};
	}

	static RuntimeManifest MakeRuntimeManifest()
	{
		auto scenes = std::vector<SceneReference> {
			MakeSceneReference({ 1 }, "MainMenu", "Scenes/MainMenu.scene.ludus"),
			MakeSceneReference({ 2 }, "Gameplay", "Scenes/Gameplay.scene.ludus")
		};

		return RuntimeManifest::Create(
			SceneId { 2 },
			std::move(scenes),
			{
				MakeScriptReference({ 11 }, "PlayerScript"),
				MakeScriptReference({ 12 }, "CameraScript")
			}
		);
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesRuntimeManifest_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.runtime.ludus";

		const auto runtimeManifest = MakeRuntimeManifest();

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.EntrySceneId, runtimeManifest.EntrySceneId);
		ASSERT_EQ(loaded.Scenes.size(), runtimeManifest.Scenes.size());
		ASSERT_EQ(loaded.Scripts.size(), runtimeManifest.Scripts.size());
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesSceneReferences_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.runtime.ludus";

		const auto runtimeManifest = MakeRuntimeManifest();

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.Scenes.size(), runtimeManifest.Scenes.size());

		for (size_t i = 0; i < runtimeManifest.Scenes.size(); ++i)
		{
			ASSERT_EQ(loaded.Scenes[i].Id, runtimeManifest.Scenes[i].Id);
			ASSERT_EQ(loaded.Scenes[i].Name, runtimeManifest.Scenes[i].Name);
			ASSERT_EQ(loaded.Scenes[i].Path, runtimeManifest.Scenes[i].Path);
		}
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesScriptReferences_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.runtime.ludus";

		const auto runtimeManifest = MakeRuntimeManifest();

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.Scripts.size(), runtimeManifest.Scripts.size());

		for (size_t i = 0; i < runtimeManifest.Scripts.size(); ++i)
		{
			ASSERT_EQ(loaded.Scripts[i].Id, runtimeManifest.Scripts[i].Id);
			ASSERT_EQ(loaded.Scripts[i].Name, runtimeManifest.Scripts[i].Name);
		}
	}

	TEST(LmlRuntimeManifestPersistence, Load_ThrowsRuntimeError_When_RuntimeManifestIsMalformed)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.runtime.ludus";

		FileSystem::WriteAllText(runtimeManifestPath, "EntrySceneId: 1, Scenes: [], Scripts: []");

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act & Assert.
		ASSERT_THROW(persistence.Load(runtimeManifestPath), std::runtime_error);
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesMinimalRuntimeManifest_When_RuntimeManifestIsMinimal)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.runtime.ludus";

		auto runtimeManifest = RuntimeManifest::Create();

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.EntrySceneId, runtimeManifest.EntrySceneId);
		ASSERT_TRUE(loaded.Scenes.empty());
		ASSERT_TRUE(loaded.Scripts.empty());
	}
}
