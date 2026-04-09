#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Serialization/ProjectManifestSchema.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::EditorTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using ProjectManifest = Ludus::Editor::Core::ProjectManifest;
	using ProjectManifestSchema = Ludus::Editor::Serialization::Schemas::ProjectManifestSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;

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

	TEST(ProjectManifestSchema, Serialize_WritesVersionProjectRootAndRuntimeManifestPath_When_ProjectManifestIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		ProjectManifest manifest;
		manifest.Version = { 7, 8, 9 };
		manifest.ProjectRoot = std::filesystem::path("Projects/Game");
		manifest.RuntimeManifestPath = std::filesystem::path("Game.runtime.ludus");

		// Act.
		ProjectManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& manifestObject = AsObject(*root);
		const auto* versionNode = FindMember(manifestObject, "Version");
		const auto* projectRootNode = FindMember(manifestObject, "ProjectRoot");
		const auto* runtimeManifestPathNode = FindMember(manifestObject, "RuntimeManifestPath");

		ASSERT_NE(versionNode, nullptr);
		ASSERT_NE(projectRootNode, nullptr);
		ASSERT_NE(runtimeManifestPathNode, nullptr);

		const auto& versionObject = AsObject(*versionNode);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(versionObject, "Major"))), manifest.Version.Major);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(versionObject, "Minor"))), manifest.Version.Minor);
		ASSERT_EQ(std::get<uint64_t>(AsValue(*FindMember(versionObject, "Patch"))), manifest.Version.Patch);

		const auto projectRoot = std::get<std::string>(AsValue(*projectRootNode));
		ASSERT_EQ(projectRoot, "Projects/Game");

		const auto runtimeManifestPath = std::get<std::string>(AsValue(*runtimeManifestPathNode));
		ASSERT_EQ(runtimeManifestPath, "Game.runtime.ludus");
	}

	TEST(ProjectManifestSchema, Serialize_WritesOnlyPersistedMembers_When_ProjectManifestIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		ProjectManifest manifest;
		manifest.ProjectRoot = std::filesystem::path("Projects/Game");
		manifest.RuntimeManifestPath = std::filesystem::path("Game.runtime.ludus");

		// Act.
		ProjectManifestSchema::Serialize(writer, manifest);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& manifestObject = AsObject(*root);
		ASSERT_EQ(manifestObject.size(), 3u);
		ASSERT_NE(FindMember(manifestObject, "Version"), nullptr);
		ASSERT_NE(FindMember(manifestObject, "ProjectRoot"), nullptr);
		ASSERT_NE(FindMember(manifestObject, "RuntimeManifestPath"), nullptr);
		ASSERT_EQ(FindMember(manifestObject, "Scenes"), nullptr);
		ASSERT_EQ(FindMember(manifestObject, "Scripts"), nullptr);
		ASSERT_EQ(FindMember(manifestObject, "EntrySceneId"), nullptr);
	}

	TEST(ProjectManifestSchema, Deserialize_ReadsVersionProjectRootAndRuntimeManifestPath_When_ArchiveIsValid)
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
		writer.Emit(Token::Key { "ProjectRoot" });
		writer.Emit(Token::String { "Projects/Game" });
		writer.Emit(Token::Key { "RuntimeManifestPath" });
		writer.Emit(Token::String { "Game.runtime.ludus" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& manifest = result.GetValue();
		ASSERT_EQ(manifest.Version.Major, 7u);
		ASSERT_EQ(manifest.Version.Minor, 8u);
		ASSERT_EQ(manifest.Version.Patch, 9u);
		ASSERT_EQ(manifest.ProjectRoot, std::filesystem::path("Projects/Game"));
		ASSERT_EQ(manifest.RuntimeManifestPath, std::filesystem::path("Game.runtime.ludus"));
	}

	TEST(ProjectManifestSchema, RoundTrip_PreservesProjectRootAndRuntimeManifestPath_When_SerializedAndDeserialized)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		ProjectManifest manifest;
		manifest.ProjectRoot = std::filesystem::path("Projects/Game");
		manifest.RuntimeManifestPath = std::filesystem::path("Game.runtime.ludus");

		// Act.
		ProjectManifestSchema::Serialize(writer, manifest);
		DomTokenStreamReader reader(document);
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& loadedManifest = result.GetValue();
		ASSERT_EQ(loadedManifest.Version.Major, manifest.Version.Major);
		ASSERT_EQ(loadedManifest.Version.Minor, manifest.Version.Minor);
		ASSERT_EQ(loadedManifest.Version.Patch, manifest.Version.Patch);
		ASSERT_EQ(loadedManifest.ProjectRoot, manifest.ProjectRoot);
		ASSERT_EQ(loadedManifest.RuntimeManifestPath, manifest.RuntimeManifestPath);
	}

	TEST(ProjectManifestSchema, Deserialize_Fails_When_RootIsNotObject)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectManifestSchema, Deserialize_Fails_When_VersionMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "ProjectRoot" });
		writer.Emit(Token::String { "Projects/Game" });
		writer.Emit(Token::Key { "RuntimeManifestPath" });
		writer.Emit(Token::String { "Game.runtime.ludus" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectManifestSchema, Deserialize_Fails_When_RuntimeManifestPathMissing)
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
		writer.Emit(Token::Key { "ProjectRoot" });
		writer.Emit(Token::String { "Projects/Game" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectManifestSchema, Deserialize_Fails_When_ProjectRootMissing)
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
		writer.Emit(Token::Key { "RuntimeManifestPath" });
		writer.Emit(Token::String { "Game.runtime.ludus" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectManifestSchema, Deserialize_Fails_When_RuntimeManifestPathHasWrongType)
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
		writer.Emit(Token::Key { "ProjectRoot" });
		writer.Emit(Token::String { "Projects/Game" });
		writer.Emit(Token::Key { "RuntimeManifestPath" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ProjectManifestSchema, Deserialize_Fails_When_ProjectRootHasWrongType)
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
		writer.Emit(Token::Key { "ProjectRoot" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "RuntimeManifestPath" });
		writer.Emit(Token::String { "Game.runtime.ludus" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ProjectManifestSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
