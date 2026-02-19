#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/Collider2DComponentSchema.h>

namespace Ludus::Tests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Collider2DComponentSchema = Ludus::Engine::Serialization::Schemas::Collider2DComponentSchema;
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

	TEST(Collider2DComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Collider2DComponent collider(
			1, 3, Ludus::Engine::Physics::Core::LayerMask(0xFF), true
		);
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		Collider2DComponentSchema::Serialize(writer, collider);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& colliderObject = AsObject(*root);

		const auto* ownerNode = FindMember(colliderObject, "OwnerHandle");
		const auto* layerNode = FindMember(colliderObject, "LayerIndex");
		const auto* collidesNode = FindMember(colliderObject, "CollidesWith");
		const auto* triggerNode = FindMember(colliderObject, "IsTrigger");

		ASSERT_NE(ownerNode, nullptr);
		ASSERT_NE(layerNode, nullptr);
		ASSERT_NE(collidesNode, nullptr);
		ASSERT_NE(triggerNode, nullptr);

		const auto ownerHandle = std::get<uint64_t>(AsValue(*ownerNode));
		const auto layerIndex = std::get<uint64_t>(AsValue(*layerNode));
		const auto collidesWith = std::get<uint64_t>(AsValue(*collidesNode));
		const auto isTrigger = std::get<bool>(AsValue(*triggerNode));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(layerIndex, 3u);
		ASSERT_EQ(collidesWith, 0xFFu);
		ASSERT_EQ(isTrigger, true);
	}

	TEST(Collider2DComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OwnerHandle" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "LayerIndex" });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::Key { "CollidesWith" });
		writer.Emit(Token::Int { 0xAA });
		writer.Emit(Token::Key { "IsTrigger" });
		writer.Emit(Token::Bool { true });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Collider2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& colliderResult = result.GetValue();
		ASSERT_EQ(colliderResult.OwnerHandle, 1u);
		ASSERT_EQ(colliderResult.LayerIndex, 2);
		ASSERT_EQ(colliderResult.CollidesWith.Value, 0xAAu);
		ASSERT_EQ(colliderResult.IsTrigger, true);
	}

	TEST(Collider2DComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
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
		const auto result = Collider2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& colliderResult = result.GetValue();
		ASSERT_EQ(colliderResult.OwnerHandle, 1u);
		ASSERT_EQ(colliderResult.LayerIndex, 0);
		ASSERT_EQ(colliderResult.CollidesWith.Value, Ludus::Engine::Physics::Core::LayerMask::GetEmpty().Value);
		ASSERT_EQ(colliderResult.IsTrigger, false);
	}

	TEST(Collider2DComponentSchema, Deserialize_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Collider2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Collider2DComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Collider2DComponentSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
