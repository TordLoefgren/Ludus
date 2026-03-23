#include "pch.h"

#include <filesystem>
#include <stdexcept>
#include <string_view>

#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/LmlRuntimeManifestPersistence.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::EngineTests::Persistence
{
	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using SceneReference = Ludus::Engine::Runtime::SceneReference;
	using ScriptReference = Ludus::Engine::Runtime::ScriptReference;
	using RuntimeManifestPersistence = Ludus::Engine::Persistence::IRuntimeManifestPersistence;
	using LmlRuntimeManifestPersistence = Ludus::Engine::Persistence::LmlRuntimeManifestPersistence;
	namespace FileSystem = Ludus::Engine::FileSystem;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Engine_Persistence_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	static SceneReference MakeSceneReference(uint64_t sceneHandle, std::string_view name, std::string_view path)
	{
		SceneReference reference;
		reference.Handle = sceneHandle;
		reference.Name = std::string(name);
		reference.Path = std::filesystem::path(path);
		return reference;
	}

	static ScriptReference MakeScriptReference(uint64_t scriptHandle, std::string_view name)
	{
		ScriptReference reference;
		reference.Handle = scriptHandle;
		reference.Name = std::string(name);
		return reference;
	}

	static RuntimeManifest MakeRuntimeManifest()
	{
		return RuntimeManifest::Create(
			222,
			{
				MakeSceneReference(111, "MainMenu", "Scenes/MainMenu.ludus.scene"),
				MakeSceneReference(222, "Gameplay", "Scenes/Gameplay.ludus.scene")
			},
			{
				MakeScriptReference(1001, "PlayerScript"),
				MakeScriptReference(1002, "CameraScript")
			}
		);
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesRuntimeManifest_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.ludus.runtime";

		const auto runtimeManifest = MakeRuntimeManifest();

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.EntrySceneHandle, runtimeManifest.EntrySceneHandle);
		ASSERT_EQ(loaded.Scenes.size(), runtimeManifest.Scenes.size());
		ASSERT_EQ(loaded.Scripts.size(), runtimeManifest.Scripts.size());
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesEntrySceneHandle_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.ludus.runtime";

		auto runtimeManifest = RuntimeManifest::Create(
			777,
			{ MakeSceneReference(777, "MainMenu", "Scenes/MainMenu.ludus.scene") }
		);

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.EntrySceneHandle, runtimeManifest.EntrySceneHandle);
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesSceneReferences_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.ludus.runtime";

		auto runtimeManifest = RuntimeManifest::Create(
			111,
			{
				MakeSceneReference(111, "MainMenu", "Scenes/MainMenu.ludus.scene"),
				MakeSceneReference(222, "Gameplay", "Scenes/Gameplay.ludus.scene")
			}
		);

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.Scenes.size(), runtimeManifest.Scenes.size());

		for (size_t i = 0; i < runtimeManifest.Scenes.size(); ++i)
		{
			ASSERT_EQ(loaded.Scenes[i].Handle, runtimeManifest.Scenes[i].Handle);
			ASSERT_EQ(loaded.Scenes[i].Name, runtimeManifest.Scenes[i].Name);
			ASSERT_EQ(loaded.Scenes[i].Path, runtimeManifest.Scenes[i].Path);
		}
	}

	TEST(LmlRuntimeManifestPersistence, SaveAndLoad_PreservesScriptReferences_When_RuntimeManifestIsValid)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.ludus.runtime";

		auto runtimeManifest = RuntimeManifest::Create(
			0,
			{ },
			{
				MakeScriptReference(1001, "PlayerScript"),
				MakeScriptReference(1002, "CameraScript")
			}
		);

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.Scripts.size(), runtimeManifest.Scripts.size());

		for (size_t i = 0; i < runtimeManifest.Scripts.size(); ++i)
		{
			ASSERT_EQ(loaded.Scripts[i].Handle, runtimeManifest.Scripts[i].Handle);
			ASSERT_EQ(loaded.Scripts[i].Name, runtimeManifest.Scripts[i].Name);
		}
	}

	TEST(LmlRuntimeManifestPersistence, Load_ThrowsRuntimeError_When_RuntimeManifestIsMalformed)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.ludus.runtime";

		FileSystem::WriteAllText(runtimeManifestPath, "EntrySceneHandle: 1, Scenes: [], Scripts: []");

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
		const auto runtimeManifestPath = tempDirectoryScoped.Path / "Game.ludus.runtime";

		auto runtimeManifest = RuntimeManifest::Create();

		LmlRuntimeManifestPersistence implementation;
		RuntimeManifestPersistence& persistence = implementation;

		// Act.
		persistence.Save(runtimeManifest, runtimeManifestPath);
		const auto loaded = persistence.Load(runtimeManifestPath);

		// Assert.
		ASSERT_EQ(loaded.EntrySceneHandle, runtimeManifest.EntrySceneHandle);
		ASSERT_TRUE(loaded.Scenes.empty());
		ASSERT_TRUE(loaded.Scripts.empty());
	}
}
