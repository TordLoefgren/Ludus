#include "pch.h"

#include <string>
#include <variant>

#include "DomTestHelpers.h"

#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Tests::Serialization::Core
{
	using Ludus::Engine::Serialization::Core::DomDocument;
	using Ludus::Engine::Serialization::Core::DomNode;
	using Ludus::Engine::Serialization::Core::DomArray;
	using Ludus::Engine::Serialization::Core::DomObject;
	using Ludus::Engine::Serialization::Core::DomValue;
	using Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using Ludus::Engine::Serialization::Core::Token;

	TEST(DomTokenStreamWriter, Emit_CreatesCorrectNode_When_TokenIsValue)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		writer.Emit(Token::Bool { true });

		// Assert.
		const DomNode* root = Root(document);
		ASSERT_NE(root, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomValue>(root->NodeData));

		const auto& value = Value(*root);
		const auto* boolValue = std::get_if<bool>(&value);
		ASSERT_NE(boolValue, nullptr);
		EXPECT_TRUE(*boolValue);
	}

	TEST(DomTokenStreamWriter, Emit_CreatesObject_When_TokensDefineSingleMember)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "foo" });
		writer.Emit(Token::Bool { true });
		writer.Emit(Token::EndObject { });

		// Assert.
		const DomNode* root = Root(document);
		ASSERT_NE(root, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(root->NodeData));

		const auto& object = Object(*root);
		ASSERT_EQ(object.size(), 1u);
		EXPECT_EQ(ObjectKeyAt(*root, 0), "foo");

		const DomNode* valueNode = ObjectValueAt(*root, 0);
		ASSERT_NE(valueNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomValue>(valueNode->NodeData));

		const auto& value = Value(*valueNode);
		const auto* boolValue = std::get_if<bool>(&value);
		ASSERT_NE(boolValue, nullptr);
		EXPECT_TRUE(*boolValue);
	}

	TEST(DomTokenStreamWriter, Emit_CreatesArray_When_TokensDefineTwoElements)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		writer.Emit(Token::StartArray {});
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::EndArray { });

		// Assert.
		const DomNode* root = Root(document);
		ASSERT_NE(root, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomArray>(root->NodeData));

		const auto& array = Array(*root);
		ASSERT_EQ(array.size(), 2u);

		const DomNode* firstValue = ArrayValueAt(*root, 0);
		ASSERT_NE(firstValue, nullptr);
		const auto* firstInt = std::get_if<int64_t>(&Value(*firstValue));
		ASSERT_NE(firstInt, nullptr);
		EXPECT_EQ(*firstInt, 1);

		const DomNode* secondValue = ArrayValueAt(*root, 1);
		ASSERT_NE(secondValue, nullptr);
		const auto* secondInt = std::get_if<int64_t>(&Value(*secondValue));
		ASSERT_NE(secondInt, nullptr);
		EXPECT_EQ(*secondInt, 2);
	}

	TEST(DomTokenStreamWriter, Emit_CreatesNestedObjectAndArrayStructure)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "items" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "id" });
		writer.Emit(Token::Int { 7 });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "id" });
		writer.Emit(Token::Int { 8 });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "empty" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });

		// Assert.
		const DomNode* root = Root(document);
		ASSERT_NE(root, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(root->NodeData));

		const auto& rootObject = Object(*root);
		ASSERT_EQ(rootObject.size(), 2u);

		const DomNode* itemsNode = ObjectValueAt(*root, 0);
		ASSERT_NE(itemsNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomArray>(itemsNode->NodeData));

		const auto& itemsArray = Array(*itemsNode);
		ASSERT_EQ(itemsArray.size(), 2u);

		const DomNode* firstItem = ArrayValueAt(*itemsNode, 0);
		ASSERT_NE(firstItem, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(firstItem->NodeData));
		const DomNode* firstId = ObjectValueAt(*firstItem, 0);
		ASSERT_NE(firstId, nullptr);
		const auto* firstIdValue = std::get_if<int64_t>(&Value(*firstId));
		ASSERT_NE(firstIdValue, nullptr);
		EXPECT_EQ(*firstIdValue, 7);

		const DomNode* secondItem = ArrayValueAt(*itemsNode, 1);
		ASSERT_NE(secondItem, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(secondItem->NodeData));
		const DomNode* secondId = ObjectValueAt(*secondItem, 0);
		ASSERT_NE(secondId, nullptr);
		const auto* secondIdValue = std::get_if<int64_t>(&Value(*secondId));
		ASSERT_NE(secondIdValue, nullptr);
		EXPECT_EQ(*secondIdValue, 8);

		const DomNode* emptyNode = ObjectValueAt(*root, 1);
		ASSERT_NE(emptyNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(emptyNode->NodeData));
		EXPECT_TRUE(Object(*emptyNode).empty());
	}

#if LUDUS_ENABLE_ASSERTS
	TEST(DomTokenStreamWriter, Emit_Dies_When_KeyWithoutObject)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act + Assert.
		ASSERT_DEATH({ writer.Emit(Token::Key { "oops" }); }, R"(Key requires an active object\.)");
	}

	TEST(DomTokenStreamWriter, Emit_Dies_When_EndObjectWithoutStart)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act + Assert.
		ASSERT_DEATH({ writer.Emit(Token::EndObject { }); }, R"(EndObject requires an active object\.)");
	}
#endif
}
