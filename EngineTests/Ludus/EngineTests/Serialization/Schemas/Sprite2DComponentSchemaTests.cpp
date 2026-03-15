#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/Sprite2DComponentSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Sprite2DComponentSchema = Ludus::Engine::Serialization::Schemas::Sprite2DComponentSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;
	using Shape = Ludus::Engine::Graphics::Shape;

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

	TEST(Sprite2DComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Sprite2DComponent sprite(1);
		sprite.Shape = Shape::Circle;
		sprite.Color = { 0.2f, 0.4f, 0.6f, 0.8f };
		sprite.Fill = false;

		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		Sprite2DComponentSchema::Serialize(writer, sprite);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& spriteObject = AsObject(*root);

		const auto* ownerNode = FindMember(spriteObject, "OwnerHandle");
		const auto* shapeNode = FindMember(spriteObject, "Shape");
		const auto* colorNode = FindMember(spriteObject, "Color");
		const auto* fillNode = FindMember(spriteObject, "Fill");

		ASSERT_NE(ownerNode, nullptr);
		ASSERT_NE(shapeNode, nullptr);
		ASSERT_NE(colorNode, nullptr);
		ASSERT_NE(fillNode, nullptr);

		const auto ownerHandle = std::get<uint64_t>(AsValue(*ownerNode));
		const auto shapeValue = std::get<std::string>(AsValue(*shapeNode));
		const auto fillValue = std::get<bool>(AsValue(*fillNode));

		const auto& colorObject = AsObject(*colorNode);
		const auto colorR = std::get<double>(AsValue(*FindMember(colorObject, "R")));
		const auto colorG = std::get<double>(AsValue(*FindMember(colorObject, "G")));
		const auto colorB = std::get<double>(AsValue(*FindMember(colorObject, "B")));
		const auto colorA = std::get<double>(AsValue(*FindMember(colorObject, "A")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(shapeValue, "Circle");
		ASSERT_EQ(fillValue, false);
		ASSERT_EQ(colorR, 0.2f);
		ASSERT_EQ(colorG, 0.4f);
		ASSERT_EQ(colorB, 0.6f);
		ASSERT_EQ(colorA, 0.8f);
	}

	TEST(Sprite2DComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "Shape" });
		writer.Emit(Token::String { "Circle" });
		writer.Emit(Token::Key { "Color" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "R" });
		writer.Emit(Token::Double { 0.2f });
		writer.Emit(Token::Key { "G" });
		writer.Emit(Token::Double { 0.4f });
		writer.Emit(Token::Key { "B" });
		writer.Emit(Token::Double { 0.6f });
		writer.Emit(Token::Key { "A" });
		writer.Emit(Token::Double { 0.8f });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "Fill" });
		writer.Emit(Token::Bool { false });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Sprite2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& spriteResult = result.GetValue();
		ASSERT_EQ(spriteResult.OwnerHandle, 1u);
		ASSERT_EQ(spriteResult.Shape, Shape::Circle);
		ASSERT_EQ(spriteResult.Color.R, 0.2f);
		ASSERT_EQ(spriteResult.Color.G, 0.4f);
		ASSERT_EQ(spriteResult.Color.B, 0.6f);
		ASSERT_EQ(spriteResult.Color.A, 0.8f);
		ASSERT_EQ(spriteResult.Fill, false);
	}

	TEST(Sprite2DComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Sprite2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& spriteResult = result.GetValue();
		ASSERT_EQ(spriteResult.OwnerHandle, 1u);
		ASSERT_EQ(spriteResult.Color.R, 1.0f);
		ASSERT_EQ(spriteResult.Color.G, 1.0f);
		ASSERT_EQ(spriteResult.Color.B, 1.0f);
		ASSERT_EQ(spriteResult.Color.A, 1.0f);
		ASSERT_EQ(spriteResult.Fill, true);
	}

	TEST(Sprite2DComponentSchema, Deserialize_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Sprite2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Sprite2DComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Sprite2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
