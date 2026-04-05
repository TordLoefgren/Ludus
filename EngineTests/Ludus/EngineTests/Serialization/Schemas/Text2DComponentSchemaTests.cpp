#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/Text2DComponentSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Text2DComponentSchema = Ludus::Engine::Serialization::Schemas::Text2DComponentSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;
	using HorizontalTextAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment;
	using EntityId = Ludus::Engine::Core::EntityId;

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

	TEST(Text2DComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Text2DComponent text(EntityId { 1 });
		text.Text = "Hello";
		text.Color = { 0.1f, 0.2f, 0.3f, 0.4f };
		text.HorizontalTextAlignment = HorizontalTextAlignment::Center;

		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		Text2DComponentSchema::Serialize(writer, text);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& textObject = AsObject(*root);

		const auto* textNode = FindMember(textObject, "Text");
		const auto* colorNode = FindMember(textObject, "Color");
		const auto* alignNode = FindMember(textObject, "HorizontalTextAlignment");

		ASSERT_NE(textNode, nullptr);
		ASSERT_NE(colorNode, nullptr);
		ASSERT_NE(alignNode, nullptr);

		const auto textValue = std::get<std::string>(AsValue(*textNode));
		const auto alignment = std::get<std::string>(AsValue(*alignNode));

		const auto& colorObject = AsObject(*colorNode);
		const auto colorR = std::get<double>(AsValue(*FindMember(colorObject, "R")));
		const auto colorG = std::get<double>(AsValue(*FindMember(colorObject, "G")));
		const auto colorB = std::get<double>(AsValue(*FindMember(colorObject, "B")));
		const auto colorA = std::get<double>(AsValue(*FindMember(colorObject, "A")));

		ASSERT_EQ(textValue, "Hello");
		ASSERT_EQ(alignment, "Center");
		ASSERT_EQ(colorR, 0.1f);
		ASSERT_EQ(colorG, 0.2f);
		ASSERT_EQ(colorB, 0.3f);
		ASSERT_EQ(colorA, 0.4f);
	}

	TEST(Text2DComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Text" });
		writer.Emit(Token::String { "Hello" });
		writer.Emit(Token::Key { "Color" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "R" });
		writer.Emit(Token::Double { 0.1f });
		writer.Emit(Token::Key { "G" });
		writer.Emit(Token::Double { 0.2f });
		writer.Emit(Token::Key { "B" });
		writer.Emit(Token::Double { 0.3f });
		writer.Emit(Token::Key { "A" });
		writer.Emit(Token::Double { 0.4f });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "HorizontalTextAlignment" });
		writer.Emit(Token::String { "Center" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Text2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& textResult = result.GetValue();
		ASSERT_EQ(textResult.OwnerId, ownerId);
		ASSERT_EQ(textResult.Text, "Hello");
		ASSERT_EQ(textResult.Color.R, 0.1f);
		ASSERT_EQ(textResult.Color.G, 0.2f);
		ASSERT_EQ(textResult.Color.B, 0.3f);
		ASSERT_EQ(textResult.Color.A, 0.4f);
		ASSERT_EQ(textResult.HorizontalTextAlignment, HorizontalTextAlignment::Center);
	}

	TEST(Text2DComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Text2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& textResult = result.GetValue();
		ASSERT_EQ(textResult.OwnerId, ownerId);
		ASSERT_EQ(textResult.Text, "");
		ASSERT_EQ(textResult.Color.R, 1.0f);
		ASSERT_EQ(textResult.Color.G, 1.0f);
		ASSERT_EQ(textResult.Color.B, 1.0f);
		ASSERT_EQ(textResult.Color.A, 1.0f);
	}

	TEST(Text2DComponentSchema, Deserialize_DefaultsFields_When_AllPayloadFieldsAreMissing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Text2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().OwnerId, ownerId);
		ASSERT_EQ(result.GetValue().Text, "");
		ASSERT_EQ(result.GetValue().Color.R, 1.0f);
		ASSERT_EQ(result.GetValue().Color.G, 1.0f);
		ASSERT_EQ(result.GetValue().Color.B, 1.0f);
		ASSERT_EQ(result.GetValue().Color.A, 1.0f);
	}

	TEST(Text2DComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Text2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
