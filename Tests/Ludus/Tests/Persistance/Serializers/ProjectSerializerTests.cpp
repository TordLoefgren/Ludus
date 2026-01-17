#include "pch.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/ProjectSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/SceneSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;

	using Project = Ludus::Engine::Core::Project;
	using Scene = Ludus::Engine::Core::Scene;

	using ProjectSerializer = Ludus::Engine::Persistance::Serializers::ProjectSerializer;
	using SceneSerializer = Ludus::Engine::Persistance::Serializers::SceneSerializer;

	using Ludus::Engine::Persistance::Archives::AsObject;
	using Ludus::Engine::Persistance::Archives::AsArray;
	using Ludus::Engine::Persistance::Archives::AsValue;

	static Scene MakeSceneWithEntities(uint32_t sceneHandle, size_t entityCount)
	{
		Scene scene;
		scene.Handle = sceneHandle;

		for (size_t i = 0; i < entityCount; ++i)
		{
			(void)scene.EntityComponentSystem.AddEntity();
		}

		return scene;
	}

	TEST(ProjectSerializer, Save_WritesProjectHeader_When_Saved)
	{
		// Arrange.
		DomSaveArchive archive;
		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		// Act.
		ProjectSerializer::Save(archive, project);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		ASSERT_TRUE(rootObject.contains("Project"));

		const auto& projectObject = AsObject(rootObject.at("Project"));
		ASSERT_TRUE(projectObject.contains("Version"));
	}

	TEST(ProjectSerializer, Save_WritesScenesArray_WithCorrectLength)
	{
		// Arrange.
		DomSaveArchive archive;
		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		project.Scenes.push_back(MakeSceneWithEntities(10, 3));
		project.Scenes.push_back(MakeSceneWithEntities(11, 5));
		project.Scenes.push_back(MakeSceneWithEntities(12, 7));

		// Act.
		ProjectSerializer::Save(archive, project);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& projectObject = AsObject(rootObject.at("Project"));
		ASSERT_TRUE(projectObject.contains("Scenes"));

		const auto& scenesArray = AsArray(projectObject.at("Scenes"));
		ASSERT_EQ(scenesArray.size(), project.Scenes.size());
	}

	TEST(ProjectSerializer, Save_WritesEachSceneUnderScenesArray)
	{
		// Arrange.
		DomSaveArchive archive;
		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		project.Scenes.push_back(MakeSceneWithEntities(100, 1));
		project.Scenes.push_back(MakeSceneWithEntities(200, 2));

		// Act.
		ProjectSerializer::Save(archive, project);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& projectObject = AsObject(rootObject.at("Project"));
		const auto& scenesArray = AsArray(projectObject.at("Scenes"));

		ASSERT_EQ(scenesArray.size(), project.Scenes.size());

		for (size_t i = 0; i < scenesArray.size(); ++i)
		{
			const auto& sceneElementObject = AsObject(scenesArray[i]);
			ASSERT_TRUE(sceneElementObject.contains("Scene"));
		}
	}

	TEST(ProjectSerializer, Load_ReadsVersionCorrectly)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Project");

		saveArchive.BeginObject("Version");
		saveArchive.WriteUint32("Major", 7);
		saveArchive.WriteUint32("Minor", 8);
		saveArchive.WriteUint32("Patch", 9);
		saveArchive.EndObject();

		saveArchive.BeginArray("Scenes");
		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		const auto& project = result.GetValue();

		ASSERT_EQ(project.Version.Major, 7);
		ASSERT_EQ(project.Version.Minor, 8);
		ASSERT_EQ(project.Version.Patch, 9);
	}

	TEST(ProjectSerializer, Load_LoadsAllScenes_When_ArchiveIsValid)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		project.Scenes.push_back(MakeSceneWithEntities(10, 3));
		project.Scenes.push_back(MakeSceneWithEntities(11, 5));

		ProjectSerializer::Save(saveArchive, project);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loadedProject = result.GetValue();
		ASSERT_EQ(loadedProject.Scenes.size(), project.Scenes.size());

		for (size_t i = 0; i < project.Scenes.size(); ++i)
		{
			ASSERT_EQ(loadedProject.Scenes[i].Handle, project.Scenes[i].Handle);
			ASSERT_EQ(
				loadedProject.Scenes[i].EntityComponentSystem.GetEntityCount(),
				project.Scenes[i].EntityComponentSystem.GetEntityCount()
			);
		}
	}

	TEST(ProjectSerializer, RoundTrip_PreservesScenes_When_SavedAndLoaded)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		Project project;
		project.Version.Major = 1;
		project.Version.Minor = 2;
		project.Version.Patch = 3;

		project.Scenes.push_back(MakeSceneWithEntities(111, 10));
		project.Scenes.push_back(MakeSceneWithEntities(222, 20));
		project.Scenes.push_back(MakeSceneWithEntities(333, 30));

		// Act.
		ProjectSerializer::Save(saveArchive, project);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loaded = result.GetValue();
		ASSERT_EQ(loaded.Version.Major, project.Version.Major);
		ASSERT_EQ(loaded.Version.Minor, project.Version.Minor);
		ASSERT_EQ(loaded.Version.Patch, project.Version.Patch);

		ASSERT_EQ(loaded.Scenes.size(), project.Scenes.size());

		for (size_t i = 0; i < project.Scenes.size(); ++i)
		{
			ASSERT_EQ(loaded.Scenes[i].Handle, project.Scenes[i].Handle);
			ASSERT_EQ(
				loaded.Scenes[i].EntityComponentSystem.GetEntityCount(),
				project.Scenes[i].EntityComponentSystem.GetEntityCount()
			);
		}
	}

	TEST(ProjectSerializer, Load_Fails_When_ProjectHeaderMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectSerializer, Load_Fails_When_ScenesMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Project");

		saveArchive.BeginObject("Version");
		saveArchive.WriteUint32("Major", 0);
		saveArchive.WriteUint32("Minor", 2);
		saveArchive.WriteUint32("Patch", 0);
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectSerializer, Load_Fails_When_ScenesIsNotArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Project");

		saveArchive.BeginObject("Version");
		saveArchive.WriteUint32("Major", 0);
		saveArchive.WriteUint32("Minor", 2);
		saveArchive.WriteUint32("Patch", 0);
		saveArchive.EndObject();

		saveArchive.BeginObject("Scenes");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectSerializer, Load_Fails_When_AnySceneFailsToLoad)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Project");

		saveArchive.BeginObject("Version");
		saveArchive.WriteUint32("Major", 0);
		saveArchive.WriteUint32("Minor", 2);
		saveArchive.WriteUint32("Patch", 0);
		saveArchive.EndObject();

		saveArchive.BeginArray("Scenes");

		{
			Scene goodScene = MakeSceneWithEntities(10, 1);
			saveArchive.BeginArrayElement();
			SceneSerializer::Save(saveArchive, goodScene);
			saveArchive.EndArrayElement();
		}

		{
			saveArchive.BeginArrayElement();
			saveArchive.EndArrayElement();
		}

		saveArchive.EndArray();
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = ProjectSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
