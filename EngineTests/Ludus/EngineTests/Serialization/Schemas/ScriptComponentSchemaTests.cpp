#include "pch.h"

#include <cstdint>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/ScriptComponentSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using ScriptComponentSchema = Ludus::Engine::Serialization::Schemas::ScriptComponentSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;
	using EntityId = Ludus::Engine::Core::EntityId;
	using ScriptId = Ludus::Engine::Core::ScriptId;

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

	TEST(ScriptComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::ScriptComponent script(EntityId { 1 }, ScriptId { 999 });
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		ScriptComponentSchema::Serialize(writer, script);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& scriptObject = AsObject(*root);
		const auto* idNode = FindMember(scriptObject, "Id");
		const auto* nameNode = FindMember(scriptObject, "Name");

		ASSERT_NE(idNode, nullptr);
		ASSERT_EQ(nameNode, nullptr);
	}

	TEST(ScriptComponentSchema, Deserialize_ReadsId_When_ComponentIsValid)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { 7 });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ScriptComponentSchema::Deserialize(reader, EntityId { 1 });

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& scriptResult = result.GetValue();
		ASSERT_EQ(scriptResult.OwnerId.Value, 1u);
		ASSERT_EQ(scriptResult.Id, ScriptId { 7 });
	}

	TEST(ScriptComponentSchema, Deserialize_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ScriptComponentSchema::Deserialize(reader, EntityId { 1 });

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(ScriptComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ScriptComponentSchema::Deserialize(reader, EntityId { 1 });

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
