#include "pch.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

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
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Token = Ludus::Engine::Serialization::Core::Token;

	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using SceneReference = Ludus::Engine::Runtime::SceneReference;
	using ScriptReference = Ludus::Engine::Runtime::ScriptReference;
	using RuntimeManifestSchema = Ludus::Engine::Serialization::Schemas::RuntimeManifestSchema;

	using Ludus::Engine::Serialization::Core::AsArray;
	using Ludus::Engine::Serialization::Core::AsObject;
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
		RuntimeManifest manifest;
		manifest.EntrySceneHandle = 222;
		manifest.Scenes.push_back(MakeSceneReference(111, "Bootstrap", "Scenes/Bootstrap.ludus.scene"));
		manifest.Scenes.push_back(MakeSceneReference(222, "Gameplay", "Scenes/Gameplay.ludus.scene"));
		manifest.Scripts.push_back(MakeScriptReference(1001, "PlayerScript"));
		manifest.Scripts.push_back(MakeScriptReference(1002, "CameraScript"));
		return manifest;
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
		RuntimeManifest manifest = MakeRuntimeManifest();
		manifest.Version = { 7, 8, 9 };

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& manifestObject = AsObject(*root);
		const auto* versionNode = FindMember(manifestObject, "Version");
		ASSERT_NE(versionNode, nullptr);

		const auto& versionObject = AsObject(*versionNode);
		const auto* majorNode = FindMember(versionObject, "Major");
		const auto* minorNode = FindMember(versionObject, "Minor");
		const auto* patchNode = FindMember(versionObject, "Patch");

		ASSERT_NE(majorNode, nullptr);
		ASSERT_NE(minorNode, nullptr);
		ASSERT_NE(patchNode, nullptr);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*majorNode)), manifest.Version.Major);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*minorNode)), manifest.Version.Minor);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*patchNode)), manifest.Version.Patch);
	}

	TEST(RuntimeManifestSchema, Serialize_WritesEntrySceneHandle_When_RuntimeManifestHasEntryScene)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		RuntimeManifest manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& manifestObject = AsObject(*root);
		const auto* entrySceneHandleNode = FindMember(manifestObject, "EntrySceneHandle");
		ASSERT_NE(entrySceneHandleNode, nullptr);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*entrySceneHandleNode)), manifest.EntrySceneHandle);
	}

	TEST(RuntimeManifestSchema, Serialize_WritesSceneReferences_When_RuntimeManifestHasScenes)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		RuntimeManifest manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& manifestObject = AsObject(*root);
		const auto* scenesNode = FindMember(manifestObject, "Scenes");
		ASSERT_NE(scenesNode, nullptr);

		const auto& scenesArray = AsArray(*scenesNode);
		ASSERT_EQ(scenesArray.size(), manifest.Scenes.size());

		const auto& firstScene = AsObject(*scenesArray[0]);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(firstScene, "Handle"))), manifest.Scenes[0].Handle);
		ASSERT_EQ(std::get<std::string>(AsValue(*FindMember(firstScene, "Name"))), manifest.Scenes[0].Name);
		ASSERT_EQ(std::get<std::string>(AsValue(*FindMember(firstScene, "Path"))), manifest.Scenes[0].Path.generic_string());
	}

	TEST(RuntimeManifestSchema, Serialize_WritesScriptReferences_When_RuntimeManifestHasScripts)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		RuntimeManifest manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& manifestObject = AsObject(*root);
		const auto* scriptsNode = FindMember(manifestObject, "Scripts");
		ASSERT_NE(scriptsNode, nullptr);

		const auto& scriptsArray = AsArray(*scriptsNode);
		ASSERT_EQ(scriptsArray.size(), manifest.Scripts.size());

		const auto& firstScript = AsObject(*scriptsArray[0]);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(firstScript, "Handle"))), manifest.Scripts[0].Handle);
		ASSERT_EQ(std::get<std::string>(AsValue(*FindMember(firstScript, "Name"))), manifest.Scripts[0].Name);
	}

	TEST(RuntimeManifestSchema, Deserialize_ReadsVersion_When_RuntimeManifestIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, 7, 8, 9);
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 123 });
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
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().Version.Major, 7u);
		ASSERT_EQ(result.GetValue().Version.Minor, 8u);
		ASSERT_EQ(result.GetValue().Version.Patch, 9u);
	}

	TEST(RuntimeManifestSchema, Deserialize_ReadsEntrySceneHandleCorrectly)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, 1, 0, 0);
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 444 });
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
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().EntrySceneHandle, 444u);
	}

	TEST(RuntimeManifestSchema, Deserialize_LoadsAllScenes_When_ArchiveIsValid)
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
		ASSERT_EQ(loadedManifest.Scenes.size(), manifest.Scenes.size());

		for (size_t i = 0; i < manifest.Scenes.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Scenes[i].Handle, manifest.Scenes[i].Handle);
			ASSERT_EQ(loadedManifest.Scenes[i].Name, manifest.Scenes[i].Name);
			ASSERT_EQ(loadedManifest.Scenes[i].Path, manifest.Scenes[i].Path);
		}
	}

	TEST(RuntimeManifestSchema, Deserialize_LoadsAllScripts_When_ArchiveIsValid)
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
		ASSERT_EQ(loadedManifest.Scripts.size(), manifest.Scripts.size());

		for (size_t i = 0; i < manifest.Scripts.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Scripts[i].Handle, manifest.Scripts[i].Handle);
			ASSERT_EQ(loadedManifest.Scripts[i].Name, manifest.Scripts[i].Name);
		}
	}

	TEST(RuntimeManifestSchema, RoundTrip_PreservesEntrySceneScenesAndScripts_When_SerializedAndDeserialized)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		const RuntimeManifest manifest = MakeRuntimeManifest();

		// Act.
		RuntimeManifestSchema::Serialize(writer, manifest);
		DomTokenStreamReader reader(document);
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loadedManifest = result.GetValue();
		ASSERT_EQ(loadedManifest.Version.Major, manifest.Version.Major);
		ASSERT_EQ(loadedManifest.Version.Minor, manifest.Version.Minor);
		ASSERT_EQ(loadedManifest.Version.Patch, manifest.Version.Patch);
		ASSERT_EQ(loadedManifest.EntrySceneHandle, manifest.EntrySceneHandle);
		ASSERT_EQ(loadedManifest.Scenes.size(), manifest.Scenes.size());
		ASSERT_EQ(loadedManifest.Scripts.size(), manifest.Scripts.size());

		for (size_t i = 0; i < manifest.Scenes.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Scenes[i].Handle, manifest.Scenes[i].Handle);
			ASSERT_EQ(loadedManifest.Scenes[i].Name, manifest.Scenes[i].Name);
			ASSERT_EQ(loadedManifest.Scenes[i].Path, manifest.Scenes[i].Path);
		}

		for (size_t i = 0; i < manifest.Scripts.size(); ++i)
		{
			ASSERT_EQ(loadedManifest.Scripts[i].Handle, manifest.Scripts[i].Handle);
			ASSERT_EQ(loadedManifest.Scripts[i].Name, manifest.Scripts[i].Name);
		}
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_RootIsNotObject)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_VersionMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 1 });
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

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_ScenesIsNotArray)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, 0, 2, 0);
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
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

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_SceneEntryIsIncomplete)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, 0, 2, 0);
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Handle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Scene1" });
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

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_ScriptsIsNotArray)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, 0, 2, 0);
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RuntimeManifestSchema, Deserialize_Fails_When_ScriptEntryIsIncomplete)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		WriteVersion(writer, 0, 2, 0);
		writer.Emit(Token::Key { "EntrySceneHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Handle" });
		writer.Emit(Token::Int { 100 });
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
