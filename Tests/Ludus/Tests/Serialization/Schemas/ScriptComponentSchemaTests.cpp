#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/ScriptComponentSchema.h>

namespace Ludus::Tests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using ScriptComponentSchema = Ludus::Engine::Serialization::Schemas::ScriptComponentSchema;
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

	TEST(ScriptComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::ScriptComponent script(1, "PlayerScript", 999);
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		ScriptComponentSchema::Serialize(writer, script);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& scriptObject = AsObject(*root);
		const auto* ownerNode = FindMember(scriptObject, "OwnerHandle");
		const auto* handleNode = FindMember(scriptObject, "Handle");
		const auto* nameNode = FindMember(scriptObject, "Name");

		ASSERT_NE(ownerNode, nullptr);
		ASSERT_NE(handleNode, nullptr);
		ASSERT_NE(nameNode, nullptr);

		const auto ownerHandle = std::get<uint64_t>(AsValue(*ownerNode));
		const auto handle = std::get<uint64_t>(AsValue(*handleNode));
		const auto name = std::get<std::string>(AsValue(*nameNode));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(handle, 999u);
		ASSERT_EQ(name, "PlayerScript");
	}

	TEST(ScriptComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Handle" });
		writer.Emit(Token::Int { 999 });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "PlayerScript" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ScriptComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& scriptResult = result.GetValue();
		ASSERT_EQ(scriptResult.OwnerHandle, 1u);
		ASSERT_EQ(scriptResult.Handle, 999u);
		ASSERT_EQ(scriptResult.Name, "PlayerScript");
	}

	TEST(ScriptComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Handle" });
		writer.Emit(Token::Int { 999 });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = ScriptComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& scriptResult = result.GetValue();
		ASSERT_EQ(scriptResult.OwnerHandle, 1u);
		ASSERT_EQ(scriptResult.Handle, 999u);
		ASSERT_EQ(scriptResult.Name, "");
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
		const auto result = ScriptComponentSchema::Deserialize(reader);

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
		const auto result = ScriptComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
