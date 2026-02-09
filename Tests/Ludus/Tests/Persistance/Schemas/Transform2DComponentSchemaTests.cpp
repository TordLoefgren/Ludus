#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Persistance/Schemas/Transform2DComponentSchema.h>
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
	using Transform2DComponentSchema = Ludus::Engine::Persistance::Schemas::Transform2DComponentSchema;
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

	TEST(Transform2DComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Transform2DComponent transform(1, { 5, 10 }, { 2, 4 }, 45);
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		Transform2DComponentSchema::Serialize(writer, transform);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& transformObject = AsObject(*root);

		const auto* ownerNode = FindMember(transformObject, "OwnerHandle");
		const auto* positionNode = FindMember(transformObject, "Position");
		const auto* scaleNode = FindMember(transformObject, "Scale");
		const auto* rotationNode = FindMember(transformObject, "Rotation");

		ASSERT_NE(ownerNode, nullptr);
		ASSERT_NE(positionNode, nullptr);
		ASSERT_NE(scaleNode, nullptr);
		ASSERT_NE(rotationNode, nullptr);

		const auto ownerHandle = std::get<uint32_t>(AsValue(*ownerNode));

		const auto& positionObject = AsObject(*positionNode);
		const auto positionX = std::get<float>(AsValue(*FindMember(positionObject, "X")));
		const auto positionY = std::get<float>(AsValue(*FindMember(positionObject, "Y")));

		const auto& scaleObject = AsObject(*scaleNode);
		const auto scaleX = std::get<float>(AsValue(*FindMember(scaleObject, "X")));
		const auto scaleY = std::get<float>(AsValue(*FindMember(scaleObject, "Y")));

		const auto rotation = std::get<float>(AsValue(*rotationNode));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(positionX, 5.0f);
		ASSERT_EQ(positionY, 10.0f);
		ASSERT_EQ(scaleX, 2.0f);
		ASSERT_EQ(scaleY, 4.0f);
		ASSERT_EQ(rotation, 45.0f);
	}

	TEST(Transform2DComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Uint32 { 1u });
		writer.Emit(Token::Key { "Position" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "X" });
		writer.Emit(Token::Float { 5.0f });
		writer.Emit(Token::Key { "Y" });
		writer.Emit(Token::Float { 10.0f });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "Scale" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "X" });
		writer.Emit(Token::Float { 2.0f });
		writer.Emit(Token::Key { "Y" });
		writer.Emit(Token::Float { 4.0f });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "Rotation" });
		writer.Emit(Token::Float { 45.0f });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Transform2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& transformResult = result.GetValue();
		ASSERT_EQ(transformResult.OwnerHandle, 1u);
		ASSERT_EQ(transformResult.Position.X, 5.0f);
		ASSERT_EQ(transformResult.Position.Y, 10.0f);
		ASSERT_EQ(transformResult.Scale.X, 2.0f);
		ASSERT_EQ(transformResult.Scale.Y, 4.0f);
		ASSERT_EQ(transformResult.Rotation, 45.0f);
	}

	TEST(Transform2DComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
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
		const auto result = Transform2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& transformResult = result.GetValue();
		ASSERT_EQ(transformResult.OwnerHandle, 1u);
		ASSERT_EQ(transformResult.Position.X, 0.0f);
		ASSERT_EQ(transformResult.Position.Y, 0.0f);
		ASSERT_EQ(transformResult.Scale.X, 1.0f);
		ASSERT_EQ(transformResult.Scale.Y, 1.0f);
		ASSERT_EQ(transformResult.Rotation, 0.0f);
	}

	TEST(Transform2DComponentSchema, Deserialize_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Transform2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Transform2DComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Transform2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
