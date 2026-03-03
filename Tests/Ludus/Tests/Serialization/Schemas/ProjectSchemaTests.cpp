#include "pch.h"

#include <cstdint>
#include <string_view>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/ProjectSchema.h>

namespace Ludus::Tests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Token = Ludus::Engine::Serialization::Core::Token;

	using Project = Ludus::Engine::Core::Project;
	using ProjectSceneReference = Ludus::Engine::Core::ProjectSceneReference;
	using ProjectSchema = Ludus::Engine::Serialization::Schemas::ProjectSchema;

	using Ludus::Engine::Serialization::Core::AsObject;
	using Ludus::Engine::Serialization::Core::AsArray;
	using Ludus::Engine::Serialization::Core::AsValue;
	using Ludus::Engine::Serialization::Core::DomNode;
	using Ludus::Engine::Serialization::Core::DomObject;

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

	static ProjectSceneReference MakeSceneReference(uint64_t sceneHandle, std::string_view name, std::string_view path)
	{
		ProjectSceneReference reference;
		reference.Handle = sceneHandle;
		reference.Name = std::string(name);
		reference.Path = std::string(path);
		return reference;
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

		project.Scenes.push_back(MakeSceneReference(10, "SceneA", "Scenes/SceneA.ludus.scene"));
		project.Scenes.push_back(MakeSceneReference(11, "SceneB", "Scenes/SceneB.ludus.scene"));
		project.Scenes.push_back(MakeSceneReference(12, "SceneC", "Scenes/SceneC.ludus.scene"));

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

		project.Scenes.push_back(MakeSceneReference(100, "Scene100", "Scenes/Scene100.ludus.scene"));
		project.Scenes.push_back(MakeSceneReference(200, "Scene200", "Scenes/Scene200.ludus.scene"));

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
		writer.Emit(Token::Int { 7 });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Int { 8 });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Int { 9 });
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

		ASSERT_EQ(project.Version.Major, 7);
		ASSERT_EQ(project.Version.Minor, 8);
		ASSERT_EQ(project.Version.Patch, 9);
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

		project.Scenes.push_back(MakeSceneReference(10, "Scene10", "Scenes/Scene10.ludus.scene"));
		project.Scenes.push_back(MakeSceneReference(11, "Scene11", "Scenes/Scene11.ludus.scene"));

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
			ASSERT_EQ(loadedProject.Scenes[i].Name, project.Scenes[i].Name);
			ASSERT_EQ(loadedProject.Scenes[i].Path, project.Scenes[i].Path);
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
		project.ActiveSceneHandle = 222;

		project.Scenes.push_back(MakeSceneReference(111, "Scene111", "Scenes/Scene111.ludus.scene"));
		project.Scenes.push_back(MakeSceneReference(222, "Scene222", "Scenes/Scene222.ludus.scene"));
		project.Scenes.push_back(MakeSceneReference(333, "Scene333", "Scenes/Scene333.ludus.scene"));

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
		ASSERT_EQ(loaded.ActiveSceneHandle, project.ActiveSceneHandle);

		ASSERT_EQ(loaded.Scenes.size(), project.Scenes.size());

		for (size_t i = 0; i < project.Scenes.size(); ++i)
		{
			ASSERT_EQ(loaded.Scenes[i].Handle, project.Scenes[i].Handle);
			ASSERT_EQ(loaded.Scenes[i].Name, project.Scenes[i].Name);
			ASSERT_EQ(loaded.Scenes[i].Path, project.Scenes[i].Path);
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
		writer.Emit(Token::Int { 0 });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Int { 0 });
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
		writer.Emit(Token::Int { 0 });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Int { 0 });
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
