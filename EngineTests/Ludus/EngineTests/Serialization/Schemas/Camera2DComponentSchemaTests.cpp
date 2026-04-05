#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/Camera2DComponentSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using EntityId = Ludus::Engine::Core::EntityId;
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using Camera2DComponentSchema = Ludus::Engine::Serialization::Schemas::Camera2DComponentSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;

	static const Ludus::Engine::Serialization::Core::DomNode* FindMember(
		const Ludus::Engine::Serialization::Core::DomObject& object,
		std::string_view key
	)
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

	TEST(Camera2DComponentSchema, Serialize_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Camera2DComponent camera(EntityId { 1 }, 10.0f, -1);
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		Camera2DComponentSchema::Serialize(writer, camera);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& cameraObject = AsObject(*root);
		const auto* sizeNode = FindMember(cameraObject, "OrthographicSize");
		const auto* priorityNode = FindMember(cameraObject, "Priority");

		ASSERT_NE(sizeNode, nullptr);
		ASSERT_NE(priorityNode, nullptr);

		const auto orthographicSize = std::get<double>(AsValue(*sizeNode));
		const auto priority = std::get<int64_t>(AsValue(*priorityNode));

		ASSERT_EQ(orthographicSize, 10.0f);
		ASSERT_EQ(priority, -1);
	}

	TEST(Camera2DComponentSchema, Deserialize_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerId = EntityId { 1 };
		const auto orthographicSize = 10.0f;
		const auto priority = -1;

		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "OrthographicSize" });
		writer.Emit(Token::Double { orthographicSize });
		writer.Emit(Token::Key { "Priority" });
		writer.Emit(Token::Int { priority });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto& result = Camera2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& cameraResult = result.GetValue();
		ASSERT_EQ(cameraResult.OwnerId, ownerId);
		ASSERT_EQ(cameraResult.OrthographicSize, orthographicSize);
		ASSERT_EQ(cameraResult.Priority, priority);
	}

	TEST(Camera2DComponentSchema, Deserialize_DefaultsOptionalFields_When_Missing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Camera2DComponentSchema::Deserialize(reader, EntityId { 1 });

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& cameraResult = result.GetValue();
		ASSERT_EQ(cameraResult.OwnerId.Value, 1u);
		ASSERT_EQ(cameraResult.OrthographicSize, 10.0f);
		ASSERT_EQ(cameraResult.Priority, -1);
	}

	TEST(Camera2DComponentSchema, Deserialize_DefaultsFields_When_AllPayloadFieldsAreMissing)
	{
		// Arrange.
		const auto ownerId = EntityId { 1 };
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Camera2DComponentSchema::Deserialize(reader, ownerId);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().OwnerId, ownerId);
		ASSERT_EQ(result.GetValue().OrthographicSize, 10.0f);
		ASSERT_EQ(result.GetValue().Priority, -1);
	}

	TEST(Camera2DComponentSchema, Deserialize_Fails_When_ObjectStartIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = Camera2DComponentSchema::Deserialize(reader, EntityId { 1 });

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
