#include "pch.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveAction.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/SceneSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using Scene = Ludus::Engine::Core::Scene;
	using SceneSerializer = Ludus::Engine::Persistance::Serializers::SceneSerializer;

	using Ludus::Engine::Persistance::Archives::AsObject;
	using Ludus::Engine::Persistance::Archives::AsArray;
	using Ludus::Engine::Persistance::Archives::AsValue;

	inline static void AssertContainsComponent(
		const Ludus::Engine::Persistance::Archives::DomNode& entity,
		std::string_view component
	)
	{
		const auto& entityObject = AsObject(entity);

		const auto entityHandle = std::get<uint32_t>(AsValue(entityObject.at("Handle")));

		const auto& componentObject = AsObject(entityObject.at(std::string(component)));
		const auto componentHandle = std::get<uint32_t>(AsValue(componentObject.at("OwnerHandle")));

		ASSERT_EQ(entityHandle, componentHandle);
	}

	TEST(SceneSerializer, Save_WritesSceneWithEntitiesArray_When_SceneHasEntities)
	{
		// Arrange.
		DomSaveArchive archive;
		Scene scene;

		const int entityCount = 10;
		for (int i = 0; i < entityCount; i++)
		{
			const auto handle = scene.EntityComponentSystem.AddEntity();
			scene.EntityComponentSystem.AttachTransform(handle);
		}

		// Act.
		SceneSerializer::Save(archive, scene);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& sceneObject = AsObject(rootObject.at("Scene"));
		const auto& entitiesArray = AsArray(sceneObject.at("Entities"));

		ASSERT_EQ(entitiesArray.size(), static_cast<size_t>(entityCount));
	}

	TEST(SceneSerializer, Save_WritesComponentsPerEntity_When_Present)
	{
		// Arrange.
		DomSaveArchive archive;
		Scene scene;

		const size_t entityCount = 10;
		for (size_t i = 0; i < entityCount; i++)
		{
			const auto handle = scene.EntityComponentSystem.AddEntity();
			scene.EntityComponentSystem.AttachCamera(handle);
			scene.EntityComponentSystem.AttachCollider(handle);
			scene.EntityComponentSystem.AttachDisplayName(handle);
			scene.EntityComponentSystem.AttachRigidBody(handle);
			scene.EntityComponentSystem.AttachSprite(handle);
			scene.EntityComponentSystem.AttachText(handle);
			scene.EntityComponentSystem.AttachTransform(handle);
		}

		// Act.
		SceneSerializer::Save(archive, scene);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& sceneObject = AsObject(rootObject.at("Scene"));
		const auto& entitiesArray = AsArray(sceneObject.at("Entities"));

		ASSERT_EQ(entitiesArray.size(), entityCount);

		for (size_t i = 0; i < entityCount; i++)
		{
			const auto& entityObject = entitiesArray[i];

			AssertContainsComponent(entityObject, "Camera2D");
			AssertContainsComponent(entityObject, "Collider2D");
			AssertContainsComponent(entityObject, "DisplayName");
			AssertContainsComponent(entityObject, "RigidBody2D");
			AssertContainsComponent(entityObject, "Sprite2D");
			AssertContainsComponent(entityObject, "Text2D");
			AssertContainsComponent(entityObject, "Transform2D");
		}
	}

	TEST(SceneSerializer, Load_CreatesEntitiesWithSameHandles_When_ArchiveIsValid)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		Scene scene;

		size_t entityCount = 10;
		for (size_t i = 0; i < entityCount; i++)
		{
			(void)scene.EntityComponentSystem.AddEntity();
		}

		SceneSerializer::Save(saveArchive, scene);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& savedEntities = scene.EntityComponentSystem.View();
		const auto& loadedEntities = result.GetValue().EntityComponentSystem.View();

		for (size_t i = 0; i < entityCount; i++)
		{
			ASSERT_EQ(savedEntities[i], loadedEntities[i]);
		}
	}

	TEST(SceneSerializer, Load_AttachesComponentsToCorrectEntity)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		Scene scene;

		const auto handle1 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachCamera(handle1);

		const auto handle2 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachCollider(handle2);

		const auto handle3 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachDisplayName(handle3);

		const auto handle4 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachRigidBody(handle4);

		const auto handle5 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachSprite(handle5);

		const auto handle6 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachText(handle6);

		const auto handle7 = scene.EntityComponentSystem.AddEntity();
		scene.EntityComponentSystem.AttachTransform(handle7);

		SceneSerializer::Save(saveArchive, scene);
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loadedECS = result.GetValue().EntityComponentSystem;
		ASSERT_TRUE(loadedECS.Cameras.ContainsOwner(handle1));
		ASSERT_TRUE(loadedECS.Colliders.ContainsOwner(handle2));
		ASSERT_TRUE(loadedECS.DisplayNames.ContainsOwner(handle3));
		ASSERT_TRUE(loadedECS.RigidBodies.ContainsOwner(handle4));
		ASSERT_TRUE(loadedECS.Sprites.ContainsOwner(handle5));
		ASSERT_TRUE(loadedECS.Texts.ContainsOwner(handle6));
		ASSERT_TRUE(loadedECS.Transforms.ContainsOwner(handle7));
	}

	TEST(SceneSerializer, RoundTrip_PreservesEntitiesAndComponents_When_SavedAndLoaded)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		Scene scene;

		size_t entityCount = 10;
		for (size_t i = 0; i < entityCount; i++)
		{
			const auto handle = scene.EntityComponentSystem.AddEntity();
			scene.EntityComponentSystem.AttachTransform(handle);
		}

		// Act.
		SceneSerializer::Save(saveArchive, scene);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		auto& loadedScene = result.GetValue();
		ASSERT_EQ(scene.EntityComponentSystem.GetEntityCount(), loadedScene.EntityComponentSystem.GetEntityCount());
	}

	TEST(SceneSerializer, Load_Fails_When_SceneHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(SceneSerializer, Load_Fails_When_EntitiesIsNotArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Scene");

		saveArchive.BeginObject("Entities");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(SceneSerializer, Load_SkipsEntity_When_EntityHandleIsMissing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Scene");

		saveArchive.WriteUint32("Handle", 0);

		saveArchive.BeginArray("Entities");

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& errors = diagnostics.GetErrors();
		const auto& warnings = diagnostics.GetWarnings();

		ASSERT_EQ(errors.size(), 1);
		const auto& error = errors[0];
		ASSERT_EQ(error.Severity, Ludus::Engine::Persistance::Archives::ArchiveSeverity::Error);
		ASSERT_EQ(error.Action, Ludus::Engine::Persistance::Archives::ArchiveAction::EntitySkipped);
		ASSERT_EQ(error.Message, std::string("Malformed entity skipped. Handle missing."));
		ASSERT_EQ(error.Path, std::string("Scene.Entities[0].Handle"));

		ASSERT_EQ(warnings.size(), 1);
		const auto& warning = warnings[0];
		ASSERT_EQ(warning.Severity, Ludus::Engine::Persistance::Archives::ArchiveSeverity::Warn);
		ASSERT_EQ(warning.Action, Ludus::Engine::Persistance::Archives::ArchiveAction::ReadFailed);
		ASSERT_EQ(warning.Message, std::string("Object did not contain a valid key: Handle"));
		ASSERT_EQ(warning.Path, std::string("Scene.Entities[0].Handle"));
	}

	TEST(SceneSerializer, Load_SkipsEntity_When_ComponentRequiredFieldMissing_And_Reports)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Scene");
		saveArchive.WriteUint32("Handle", 0);

		saveArchive.BeginArray("Entities");

		saveArchive.BeginArrayElement();
		saveArchive.WriteUint32("Handle", 123);

		saveArchive.BeginObject("Transform2D");
		saveArchive.EndObject();

		saveArchive.EndArrayElement();

		saveArchive.EndArray();
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& errors = diagnostics.GetErrors();
		const auto& warnings = diagnostics.GetWarnings();

		ASSERT_EQ(errors.size(), 1);
		const auto& error = errors[0];
		ASSERT_EQ(error.Action, Ludus::Engine::Persistance::Archives::ArchiveAction::EntitySkipped);
		ASSERT_EQ(error.Severity, Ludus::Engine::Persistance::Archives::ArchiveSeverity::Error);
		ASSERT_EQ(error.Message, std::string("Malformed entity skipped. Transform2D was malformed: Object did not contain a valid key: OwnerHandle"));
		ASSERT_EQ(error.Path, std::string("Scene.Entities[0].Transform2D"));

		ASSERT_EQ(warnings.size(), 0);
	}

	TEST(SceneSerializer, Load_KeepsOtherEntities_When_OneEntityIsCorrupt)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject("Scene");
		saveArchive.WriteUint32("Handle", 0);

		saveArchive.BeginArray("Entities");

		saveArchive.BeginArrayElement();
		saveArchive.WriteUint32("Handle", 0);

		saveArchive.BeginObject("Transform2D");
		saveArchive.EndObject();

		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteUint32("Handle", 1);
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteUint32("Handle", 2);
		saveArchive.EndArrayElement();

		saveArchive.EndArray();
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = SceneSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().EntityComponentSystem.View().size(), 2);

		const auto& errors = diagnostics.GetErrors();
		const auto& warnings = diagnostics.GetWarnings();

		ASSERT_EQ(errors.size(), 1);
		const auto& error = errors[0];
		ASSERT_EQ(error.Action, Ludus::Engine::Persistance::Archives::ArchiveAction::EntitySkipped);
		ASSERT_EQ(error.Severity, Ludus::Engine::Persistance::Archives::ArchiveSeverity::Error);
		ASSERT_EQ(error.Message, std::string("Malformed entity skipped. Transform2D was malformed: Object did not contain a valid key: OwnerHandle"));
		ASSERT_EQ(error.Path, std::string("Scene.Entities[0].Transform2D"));

		ASSERT_EQ(warnings.size(), 0);
	}
}
