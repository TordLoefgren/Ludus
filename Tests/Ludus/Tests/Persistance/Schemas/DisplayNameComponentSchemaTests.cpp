#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Persistance/Schemas/DisplayNameComponentSchema.h>
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
	using DisplayNameComponentSchema = Ludus::Engine::Persistance::Schemas::DisplayNameComponentSchema;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	using Ludus::Engine::Persistance::Serialization::AsObject;
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

	TEST(DisplayNameComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::DisplayNameComponent displayName(1, "Name");
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		DisplayNameComponentSchema::Serialize(writer, displayName);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& displayNameObject = AsObject(*root);
		const auto* ownerNode = FindMember(displayNameObject, "OwnerHandle");
		const auto* valueNode = FindMember(displayNameObject, "Value");

		ASSERT_NE(ownerNode, nullptr);
		ASSERT_NE(valueNode, nullptr);

		const auto ownerHandle = std::get<uint32_t>(AsValue(*ownerNode));
		const auto value = std::get<std::string>(AsValue(*valueNode));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(value, "Name");
	}

	TEST(DisplayNameComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Uint32 { 1u });
		writer.Emit(Token::Key { "Value" });
		writer.Emit(Token::String { "Name" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& displayNameResult = result.GetValue();
		ASSERT_EQ(displayNameResult.OwnerHandle, 1u);
		ASSERT_EQ(displayNameResult.Value, "Name");
	}

	TEST(DisplayNameComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Uint32 { 1u });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& displayNameResult = result.GetValue();
		ASSERT_EQ(displayNameResult.OwnerHandle, 1u);
		ASSERT_EQ(displayNameResult.Value, "");
	}

	TEST(DisplayNameComponentSchema, Deserialize_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(DisplayNameComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
