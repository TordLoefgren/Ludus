#include "pch.h"

#include <cstdint>
#include <string_view>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistance/Schemas/ProjectSchema.h>
#include <Ludus/Engine/Persistance/Schemas/SceneSchema.h>
#include <Ludus/Engine/Persistance/Serialization/DomDocument.h>
#include <Ludus/Engine/Persistance/Serialization/DomNode.h>
#include <Ludus/Engine/Persistance/Serialization/DomTokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/DomTokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/Token.h>

namespace Ludus::Tests::Persistance::Schemas
{
	using DomDocument = Ludus::Engine::Persistance::Serialization::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Persistance::Serialization::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Persistance::Serialization::DomTokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	using Project = Ludus::Engine::Core::Project;
	using Scene = Ludus::Engine::Core::Scene;
	using ProjectSchema = Ludus::Engine::Persistance::Schemas::ProjectSchema;

	using Ludus::Engine::Persistance::Serialization::AsObject;
	using Ludus::Engine::Persistance::Serialization::AsArray;
	using Ludus::Engine::Persistance::Serialization::AsValue;
	using Ludus::Engine::Persistance::Serialization::DomNode;
	using Ludus::Engine::Persistance::Serialization::DomObject;

	static const DomNode* FindMember(const DomObject& object, std::string_view key)
	{
		for (const auto& [memberKey, memberValue] : object)
		{
			if (memberKey == key)
			{
				return memberValue.get();
			}
		}

		return nullptr;
	}

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

	TEST(ProjectSchema, Serialize_WritesProjectHeader_When_Serialized)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		// Act.
		ProjectSchema::Serialize(writer, project);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& projectObject = AsObject(*root);
		const auto* versionNode = FindMember(projectObject, "Version");
		ASSERT_NE(versionNode, nullptr);
	}

	TEST(ProjectSchema, Serialize_WritesScenesArray_WithCorrectLength)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		project.Scenes.push_back(MakeSceneWithEntities(10, 3));
		project.Scenes.push_back(MakeSceneWithEntities(11, 5));
		project.Scenes.push_back(MakeSceneWithEntities(12, 7));

		// Act.
		ProjectSchema::Serialize(writer, project);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& projectObject = AsObject(*root);
		const auto* scenesNode = FindMember(projectObject, "Scenes");
		ASSERT_NE(scenesNode, nullptr);

		const auto& scenesArray = AsArray(*scenesNode);
		ASSERT_EQ(scenesArray.size(), project.Scenes.size());
	}

	TEST(ProjectSchema, Serialize_WritesEachSceneUnderScenesArray)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		project.Scenes.push_back(MakeSceneWithEntities(100, 1));
		project.Scenes.push_back(MakeSceneWithEntities(200, 2));

		// Act.
		ProjectSchema::Serialize(writer, project);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& projectObject = AsObject(*root);
		const auto* scenesNode = FindMember(projectObject, "Scenes");
		ASSERT_NE(scenesNode, nullptr);

		const auto& scenesArray = AsArray(*scenesNode);
		ASSERT_EQ(scenesArray.size(), project.Scenes.size());

		for (size_t i = 0; i < scenesArray.size(); ++i)
		{
			const auto& sceneObject = AsObject(*scenesArray[i]);
			ASSERT_NE(FindMember(sceneObject, "Handle"), nullptr);
		}
	}

	TEST(ProjectSchema, Deserialize_ReadsVersionCorrectly)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Version" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Major" });
		writer.Emit(Token::Uint32 { 7u });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Uint32 { 8u });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Uint32 { 9u });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		const auto& project = result.GetValue();

		ASSERT_EQ(project.Version.Major, 7u);
		ASSERT_EQ(project.Version.Minor, 8u);
		ASSERT_EQ(project.Version.Patch, 9u);
	}

	TEST(ProjectSchema, Deserialize_LoadsAllScenes_When_ArchiveIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		Project project;
		project.Version.Major = 0;
		project.Version.Minor = 2;
		project.Version.Patch = 0;

		project.Scenes.push_back(MakeSceneWithEntities(10, 3));
		project.Scenes.push_back(MakeSceneWithEntities(11, 5));

		ProjectSchema::Serialize(writer, project);
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectSchema::Deserialize(reader);

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

	TEST(ProjectSchema, RoundTrip_PreservesScenes_When_SerializedAndDeserialized)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		Project project;
		project.Version.Major = 1;
		project.Version.Minor = 2;
		project.Version.Patch = 3;

		project.Scenes.push_back(MakeSceneWithEntities(111, 10));
		project.Scenes.push_back(MakeSceneWithEntities(222, 20));
		project.Scenes.push_back(MakeSceneWithEntities(333, 30));

		// Act.
		ProjectSchema::Serialize(writer, project);
		DomTokenStreamReader reader(document);
		const auto result = ProjectSchema::Deserialize(reader);

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

	TEST(ProjectSchema, Deserialize_Fails_When_RootIsNotObject)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectSchema, Deserialize_Fails_When_VersionMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectSchema, Deserialize_Fails_When_ScenesIsNotArray)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Version" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Major" });
		writer.Emit(Token::Uint32 { 0u });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Uint32 { 2u });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Uint32 { 0u });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectSchema, Deserialize_Fails_When_AnySceneFailsToLoad)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Version" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Major" });
		writer.Emit(Token::Uint32 { 0u });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Uint32 { 2u });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Uint32 { 0u });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
