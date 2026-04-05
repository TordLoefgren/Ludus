#include "pch.h"

#include <cstdint>
#include <string_view>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/SceneSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Scene = Ludus::Engine::Core::Scene;
	using SceneSchema = Ludus::Engine::Serialization::Schemas::SceneSchema;
	using EntityId = Ludus::Engine::Core::EntityId;
	using SceneId = Ludus::Engine::Core::SceneId;
	using ScriptId = Ludus::Engine::Core::ScriptId;
	using Token = Ludus::Engine::Serialization::Core::Token;

	using Ludus::Engine::Serialization::Core::AsObject;
	using Ludus::Engine::Serialization::Core::AsArray;
	using Ludus::Engine::Serialization::Core::AsValue;
	using Ludus::Engine::Serialization::Core::DomNode;
	using Ludus::Engine::Serialization::Core::DomObject;

	static SceneId GetId()
	{
		Ludus::Engine::Core::Random random;
		return { random.NextId() };
	}

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

	static void AssertContainsComponent(const DomNode& entity, std::string_view component)
	{
		const auto& entityObject = AsObject(entity);

		const auto* componentNode = FindMember(entityObject, component);
		ASSERT_NE(componentNode, nullptr);

		const auto& componentObject = AsObject(*componentNode);
		ASSERT_EQ(componentObject.empty(), false);
	}

	TEST(SceneSchema, Serialize_WritesSceneWithEntitiesArray_When_SceneHasEntities)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Scene scene(GetId());

		const int entityCount = 10;
		for (int i = 0; i < entityCount; i++)
		{
			const auto handle = scene.EntityComponentSystem.AddEntity();
			scene.EntityComponentSystem.AttachTransform(handle);
		}

		// Act.
		SceneSchema::Serialize(writer, scene);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& sceneObject = AsObject(*root);
		const auto* entitiesNode = FindMember(sceneObject, "Entities");
		ASSERT_NE(entitiesNode, nullptr);

		const auto& entitiesArray = AsArray(*entitiesNode);
		ASSERT_EQ(entitiesArray.size(), static_cast<size_t>(entityCount));
	}

	TEST(SceneSchema, Serialize_WritesComponentsPerEntity_When_Present)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Scene scene(GetId());

		const size_t entityCount = 5;
		for (size_t i = 0; i < entityCount; i++)
		{
			const auto handle = scene.EntityComponentSystem.AddEntity();
			scene.EntityComponentSystem.AttachCamera(handle);
			scene.EntityComponentSystem.AttachCollider(handle);
			scene.EntityComponentSystem.AttachDisplayName(handle);
			scene.EntityComponentSystem.AttachRigidBody(handle);
			scene.EntityComponentSystem.AttachScript(handle);
			scene.EntityComponentSystem.AttachSprite(handle);
			scene.EntityComponentSystem.AttachText(handle);
			scene.EntityComponentSystem.AttachTransform(handle);
		}

		// Act.
		SceneSchema::Serialize(writer, scene);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& sceneObject = AsObject(*root);
		const auto* entitiesNode = FindMember(sceneObject, "Entities");
		ASSERT_NE(entitiesNode, nullptr);

		const auto& entitiesArray = AsArray(*entitiesNode);
		ASSERT_EQ(entitiesArray.size(), entityCount);

		for (size_t i = 0; i < entityCount; i++)
		{
			const auto& entityObject = *entitiesArray[i];

			AssertContainsComponent(entityObject, "Camera2D");
			AssertContainsComponent(entityObject, "Collider2D");
			AssertContainsComponent(entityObject, "DisplayName");
			AssertContainsComponent(entityObject, "RigidBody2D");
			AssertContainsComponent(entityObject, "Script");
			AssertContainsComponent(entityObject, "Sprite2D");
			AssertContainsComponent(entityObject, "Text2D");
			AssertContainsComponent(entityObject, "Transform2D");
		}
	}

	TEST(SceneSchema, Deserialize_CreatesEntitiesWithSameIds_When_ArchiveIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Scene scene(GetId());

		const size_t entityCount = 10;
		for (size_t i = 0; i < entityCount; i++)
		{
			(void)scene.EntityComponentSystem.AddEntity();
		}

		SceneSchema::Serialize(writer, scene);
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& savedEntities = scene.EntityComponentSystem.View();
		const auto& loadedEntities = result.GetValue().EntityComponentSystem.View();

		ASSERT_EQ(savedEntities.size(), loadedEntities.size());

		for (size_t i = 0; i < entityCount; i++)
		{
			ASSERT_EQ(savedEntities[i], loadedEntities[i]);
		}
	}

	TEST(SceneSchema, Deserialize_AttachesComponentsToCorrectEntity)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Scene scene(GetId());

		const auto entityId1 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachCamera(entityId1);

		const auto entityId2 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachCollider(entityId2);

		const auto entityId3 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachDisplayName(entityId3);

		const auto entityId4 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachRigidBody(entityId4);

		const auto entityId5 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachSprite(entityId5);

		const auto entityId6 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachText(entityId6);

		const auto entityId7 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachScript(entityId7, ScriptId { 1 });

		const auto entityId8 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachTransform(entityId8);

		SceneSchema::Serialize(writer, scene);
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loadedEcs = result.GetValue().EntityComponentSystem;
		const auto loadedEntities = loadedEcs.View();
		ASSERT_EQ(loadedEntities.size(), 8u);
		ASSERT_TRUE(loadedEcs.Cameras.ContainsOwner(loadedEntities[0].Id));
		ASSERT_TRUE(loadedEcs.Colliders.ContainsOwner(loadedEntities[1].Id));
		ASSERT_TRUE(loadedEcs.DisplayNames.ContainsOwner(loadedEntities[2].Id));
		ASSERT_TRUE(loadedEcs.RigidBodies.ContainsOwner(loadedEntities[3].Id));
		ASSERT_TRUE(loadedEcs.Sprites.ContainsOwner(loadedEntities[4].Id));
		ASSERT_TRUE(loadedEcs.Texts.ContainsOwner(loadedEntities[5].Id));
		ASSERT_TRUE(loadedEcs.Scripts.ContainsOwner(loadedEntities[6].Id));
		ASSERT_TRUE(loadedEcs.Transforms.ContainsOwner(loadedEntities[7].Id));
	}

	TEST(SceneSchema, RoundTrip_PreservesEntitiesAndComponents_When_SavedAndLoaded)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		Scene scene(GetId());

		const size_t entityCount = 10;
		for (size_t i = 0; i < entityCount; i++)
		{
			const auto handle = scene.EntityComponentSystem.AddEntity();
			scene.EntityComponentSystem.AttachTransform(handle);
		}

		// Act.
		SceneSchema::Serialize(writer, scene);
		DomTokenStreamReader reader(document);
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loadedScene = result.GetValue();
		ASSERT_EQ(scene.EntityComponentSystem.GetEntityCount(), loadedScene.EntityComponentSystem.GetEntityCount());
	}

	TEST(SceneSchema, Deserialize_Fails_When_RootIsNotObject)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(SceneSchema, Deserialize_Fails_When_EntitiesIsNotArray)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Entities" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(SceneSchema, Deserialize_SkipsEntity_When_EntityIdIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Entities" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().EntityComponentSystem.GetEntityCount(), 0u);
	}

	TEST(SceneSchema, Deserialize_DefaultsComponent_When_ComponentPayloadIsEmptyObject)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Entities" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Transform2D" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = SceneSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().EntityComponentSystem.GetEntityCount(), 1u);
		EXPECT_TRUE(result.GetValue().EntityComponentSystem.Transforms.ContainsOwner(EntityId { 1 }));
	}
}
