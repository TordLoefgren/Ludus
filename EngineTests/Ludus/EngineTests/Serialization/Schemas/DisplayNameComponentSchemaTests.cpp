#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/DisplayNameComponentSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using EntityId = Ludus::Engine::Core::EntityId;
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using DisplayNameComponentSchema = Ludus::Engine::Serialization::Schemas::DisplayNameComponentSchema;
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

	TEST(DisplayNameComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::DisplayNameComponent displayName(EntityId { 1 }, "Name");
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		DisplayNameComponentSchema::Serialize(writer, displayName);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& displayNameObject = AsObject(*root);
		const auto* nameNode = FindMember(displayNameObject, "Name");

		ASSERT_NE(nameNode, nullptr);

		const auto value = std::get<std::string>(AsValue(*nameNode));

		ASSERT_EQ(value, "Name");
	}

	TEST(DisplayNameComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { "Name" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& displayNameResult = result.GetValue();
		ASSERT_EQ(displayNameResult.OwnerId, ownerId);
		ASSERT_EQ(displayNameResult.Name, "Name");
	}

	TEST(DisplayNameComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& displayNameResult = result.GetValue();
		ASSERT_EQ(displayNameResult.OwnerId, ownerId);
		ASSERT_EQ(displayNameResult.Name, "");
	}

	TEST(DisplayNameComponentSchema, Deserialize_DefaultsFields_When_AllPayloadFieldsAreMissing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().OwnerId, ownerId);
		ASSERT_EQ(result.GetValue().Name, "");
	}

	TEST(DisplayNameComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = DisplayNameComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
