#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/RigidBody2DComponentSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using EntityId = Ludus::Engine::Core::EntityId;
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using RigidBody2DComponentSchema = Ludus::Engine::Serialization::Schemas::RigidBody2DComponentSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;
	using BodyType = Ludus::Engine::Physics::Core::BodyType;

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

	TEST(RigidBody2DComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::RigidBody2DComponent rigidBody(1);
		rigidBody.Velocity = { 3.0f, 4.0f };
		rigidBody.GravityScale = 2.0f;
		rigidBody.Mass = 5.0f;
		rigidBody.BodyType = BodyType::Kinematic;

		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		RigidBody2DComponentSchema::Serialize(writer, rigidBody);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& rigidBodyObject = AsObject(*root);

		const auto* velocityNode = FindMember(rigidBodyObject, "Velocity");
		const auto* gravityNode = FindMember(rigidBodyObject, "GravityScale");
		const auto* massNode = FindMember(rigidBodyObject, "Mass");
		const auto* typeNode = FindMember(rigidBodyObject, "BodyType");

		ASSERT_NE(velocityNode, nullptr);
		ASSERT_NE(gravityNode, nullptr);
		ASSERT_NE(massNode, nullptr);
		ASSERT_NE(typeNode, nullptr);

		const auto gravityScale = std::get<double>(AsValue(*gravityNode));
		const auto mass = std::get<double>(AsValue(*massNode));
		const auto typeValue = std::get<std::string>(AsValue(*typeNode));

		const auto& velocityObject = AsObject(*velocityNode);
		const auto velocityX = std::get<double>(AsValue(*FindMember(velocityObject, "X")));
		const auto velocityY = std::get<double>(AsValue(*FindMember(velocityObject, "Y")));

		ASSERT_EQ(velocityX, 3.0f);
		ASSERT_EQ(velocityY, 4.0f);
		ASSERT_EQ(gravityScale, 2.0f);
		ASSERT_EQ(mass, 5.0f);
		ASSERT_EQ(typeValue, "Kinematic");
	}

	TEST(RigidBody2DComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Velocity" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "X" });
		writer.Emit(Token::Double { 3.0f });
		writer.Emit(Token::Key { "Y" });
		writer.Emit(Token::Double { 4.0f });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "GravityScale" });
		writer.Emit(Token::Double { 2.0f });
		writer.Emit(Token::Key { "Mass" });
		writer.Emit(Token::Double { 5.0f });
		writer.Emit(Token::Key { "BodyType" });
		writer.Emit(Token::String { "Kinematic" });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RigidBody2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& rigidBodyResult = result.GetValue();
		ASSERT_EQ(rigidBodyResult.OwnerId, ownerId);
		ASSERT_EQ(rigidBodyResult.Velocity.X, 3.0f);
		ASSERT_EQ(rigidBodyResult.Velocity.Y, 4.0f);
		ASSERT_EQ(rigidBodyResult.GravityScale, 2.0f);
		ASSERT_EQ(rigidBodyResult.Mass, 5.0f);
		ASSERT_EQ(rigidBodyResult.BodyType, BodyType::Kinematic);
	}

	TEST(RigidBody2DComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RigidBody2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& rigidBodyResult = result.GetValue();
		ASSERT_EQ(rigidBodyResult.OwnerId, ownerId);
		ASSERT_EQ(rigidBodyResult.Velocity.X, 0.0f);
		ASSERT_EQ(rigidBodyResult.Velocity.Y, 0.0f);
		ASSERT_EQ(rigidBodyResult.GravityScale, 1.0f);
		ASSERT_EQ(rigidBodyResult.Mass, 1.0f);
	}

	TEST(RigidBody2DComponentSchema, Deserialize_DefaultsFields_When_AllPayloadFieldsAreMissing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RigidBody2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().OwnerId, ownerId);
		ASSERT_EQ(result.GetValue().Velocity.X, 0.0f);
		ASSERT_EQ(result.GetValue().Velocity.Y, 0.0f);
		ASSERT_EQ(result.GetValue().GravityScale, 1.0f);
		ASSERT_EQ(result.GetValue().Mass, 1.0f);
	}

	TEST(RigidBody2DComponentSchema, Deserialize_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		const EntityId ownerId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RigidBody2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
