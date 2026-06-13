#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/RuntimeManifestSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomNode = Ludus::Engine::Serialization::Core::DomNode;
	using DomObject = Ludus::Engine::Serialization::Core::DomObject;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using RuntimeManifestSchema = Ludus::Engine::Serialization::Schemas::RuntimeManifestSchema;
	using SceneReference = Ludus::Engine::Runtime::SceneReference;
	using ScriptReference = Ludus::Engine::Runtime::ScriptReference;
	using AssetReference = Ludus::Engine::Runtime::AssetReference;
	using AssetType = Ludus::Engine::Core::AssetType;
	using SceneId = Ludus::Engine::Core::SceneId;
	using ScriptId = Ludus::Engine::Core::ScriptId;
	using AssetId = Ludus::Engine::Core::AssetId;
	using Token = Ludus::Engine::Serialization::Core::Token;

	using Ludus::Engine::Serialization::Core::AsArray;
	using Ludus::Engine::Serialization::Core::AsObject;
	using Ludus::Engine::Serialization::Core::AsValue;

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

	static ScriptReference MakeScriptReference(
		ScriptId id,
		std::string_view name
	)
	{
		return {
			.Id = id,
			.Name = std::string(name)
		};
	}

	static AssetReference MakeAssetReference(
		AssetId id,
		AssetType type,
		std::string_view path
	)
	{
		return {
			.Id = id,
			.Type = type,
			.Path = std::filesystem::path(path)
		};
	}

	static RuntimeManifest MakeRuntimeManifest()
	{
		return RuntimeManifest::Create(
			SceneId { 2 },
			{
				MakeSceneReference({ 1 }, "Bootstrap", "Scenes/Bootstrap.scene.ludus"),
				MakeSceneReference({ 2 }, "Gameplay", "Scenes/Gameplay.scene.ludus")
			},
			{
				MakeScriptReference({ 10 }, "PlayerScript"),
				MakeScriptReference({ 20 }, "CameraScript")
			},
			{
				MakeAssetReference({ 100 }, AssetType::Texture2D, "Assets/Sprites/Player.png")
			}
		);
	}

	static void WriteSchemaRevision(
		DomTokenStreamWriter& writer,
		int64_t revision = RuntimeManifest::CurrentSchemaRevision
	)
	{
		writer.Emit(Token::Key { "SchemaRevision" });
		writer.Emit(Token::Int { revision });
	}

	TEST(RuntimeManifestSchema, Serialize_WritesSchemaRevision_When_RuntimeManifestIsSerialized)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		const auto manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto& manifestObject = AsObject(*document.GetRoot());
		ASSERT_EQ(
			std::get<uint64_t>(AsValue(*FindMember(manifestObject, "SchemaRevision"))),
			manifest.SchemaRevision
		);
	}

	TEST(RuntimeManifestSchema, Serialize_WritesIds_When_RuntimeManifestHasReferences)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		const RuntimeManifest manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto& manifestObject = AsObject(*document.GetRoot());
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(manifestObject, "EntrySceneId"))), manifest.EntrySceneId.Value);

		const auto& scenesArray = AsArray(*FindMember(manifestObject, "Scenes"));
		const auto& firstScene = AsObject(*scenesArray[0]);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(firstScene, "Id"))), manifest.Scenes[0].Id.Value);

		const auto& scriptsArray = AsArray(*FindMember(manifestObject, "Scripts"));
		const auto& firstScript = AsObject(*scriptsArray[0]);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(firstScript, "Id"))), manifest.Scripts[0].Id.Value);
	}

	TEST(RuntimeManifestSchema, Deserialize_LoadsManifest_When_RuntimeManifestIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		const RuntimeManifest manifest = MakeRuntimeManifest();
		RuntimeManifestSchema::Serialize(writer, manifest);
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		const auto& loadedManifest = result.GetValue();
		ASSERT_EQ(loadedManifest.SchemaRevision, manifest.SchemaRevision);
		ASSERT_EQ(loadedManifest.EntrySceneId, manifest.EntrySceneId);
		ASSERT_EQ(loadedManifest.Scenes.size(), manifest.Scenes.size());
		ASSERT_EQ(loadedManifest.Scripts.size(), manifest.Scripts.size());
		ASSERT_EQ(loadedManifest.Assets.size(), manifest.Assets.size());

		for (size_t i = 0; i < manifest.Scenes.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Scenes[i].Id, manifest.Scenes[i].Id);
			ASSERT_EQ(loadedManifest.Scenes[i].Name, manifest.Scenes[i].Name);
			ASSERT_EQ(loadedManifest.Scenes[i].Path, manifest.Scenes[i].Path);
		}

		for (size_t i = 0; i < manifest.Scripts.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Scripts[i].Id, manifest.Scripts[i].Id);
			ASSERT_EQ(loadedManifest.Scripts[i].Name, manifest.Scripts[i].Name);
		}

		for (size_t i = 0; i < manifest.Assets.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Assets[i].Id, manifest.Assets[i].Id);
			ASSERT_EQ(loadedManifest.Assets[i].Type, manifest.Assets[i].Type);
			ASSERT_EQ(loadedManifest.Assets[i].Path, manifest.Assets[i].Path);
		}
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_SchemaRevisionMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_SchemaRevisionIsUnsupported)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteSchemaRevision(writer, 2);
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 0 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_LegacyVersionIsUsed)
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
		writer.Emit(Token::Int { 3 });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Int { 0 });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 0 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_EntrySceneIdDoesNotMatchSceneReference)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteSchemaRevision(writer);
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 99 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Scene" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Scenes/Scene.scene.ludus" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_LoadsEmptyAssets_When_AssetsKeyIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteSchemaRevision(writer);
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Scene" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Scenes/Scene.scene.ludus" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_TRUE(result.GetValue().Assets.empty());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_AssetTypeIsUnknown)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteSchemaRevision(writer);
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Scene" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Scenes/Scene.scene.ludus" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Assets" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 100 });
		writer.Emit(Token::Key { "Type" });
		writer.Emit(Token::String { "Unknown" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Assets/Sprites/Player.png" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_AssetTypeStringIsInvalid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteSchemaRevision(writer);
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Scene" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Scenes/Scene.scene.ludus" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Assets" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 100 });
		writer.Emit(Token::Key { "Type" });
		writer.Emit(Token::String { "Texture3D" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Assets/Sprites/Player.png" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_AssetIdUsesBuiltInRange)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteSchemaRevision(writer);
		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 1 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Scene" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Scenes/Scene.scene.ludus" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Assets" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { Ludus::Engine::Core::BuiltInAssetIds::MissingTexture.Value });
		writer.Emit(Token::Key { "Type" });
		writer.Emit(Token::String { "Texture2D" });
		writer.Emit(Token::Key { "Path" });
		writer.Emit(Token::String { "Assets/Sprites/Player.png" });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
