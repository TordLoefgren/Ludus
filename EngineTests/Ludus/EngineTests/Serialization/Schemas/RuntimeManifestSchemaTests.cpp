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
	using SceneId = Ludus::Engine::Core::SceneId;
	using ScriptId = Ludus::Engine::Core::ScriptId;
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

	static RuntimeManifest MakeRuntimeManifest()
	{
		return RuntimeManifest::Create(
			SceneId { 2 },
			{
				MakeSceneReference({ 1 }, "Bootstrap", "Scenes/Bootstrap.ludus.scene"),
				MakeSceneReference({ 2 }, "Gameplay", "Scenes/Gameplay.ludus.scene")
			},
			{
				MakeScriptReference({ 10 }, "PlayerScript"),
				MakeScriptReference({ 20 }, "CameraScript")
			}
		);
	}

	static void WriteVersion(DomTokenStreamWriter& writer, int64_t major, int64_t minor, int64_t patch)
	{
		writer.Emit(Token::Key { "Version" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Major" });
		writer.Emit(Token::Int { major });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Int { minor });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Int { patch });
		writer.Emit(Token::EndObject { });
	}

	TEST(RuntimeManifestSchema, Serialize_WritesVersion_When_RuntimeManifestIsSerialized)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		const RuntimeManifest manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto& manifestObject = AsObject(*document.GetRoot());
		const auto& versionObject = AsObject(*FindMember(manifestObject, "Version"));
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(versionObject, "Major"))), manifest.Version.Major);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(versionObject, "Minor"))), manifest.Version.Minor);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(versionObject, "Patch"))), manifest.Version.Patch);
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
		ASSERT_EQ(loadedManifest.EntrySceneId, manifest.EntrySceneId);
		ASSERT_EQ(loadedManifest.Scenes.size(), manifest.Scenes.size());
		ASSERT_EQ(loadedManifest.Scripts.size(), manifest.Scripts.size());

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
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_VersionMissing)
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

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_EntrySceneIdDoesNotMatchSceneReference)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, RuntimeManifest::CurrentVersion.Major, RuntimeManifest::CurrentVersion.Minor, RuntimeManifest::CurrentVersion.Patch);
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
		writer.Emit(Token::String { "Scenes/Scene.ludus.scene" });
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
}
