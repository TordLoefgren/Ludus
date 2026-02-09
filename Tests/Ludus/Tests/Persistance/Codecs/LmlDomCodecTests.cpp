#include "pch.h"

#include <functional>
#include <string>
#include <string_view>
#include <variant>

#include <Ludus/Engine/Persistance/Codecs/LmlDomCodec.h>
#include <Ludus/Engine/Persistance/Serialization/DomDocument.h>
#include <Ludus/Engine/Persistance/Serialization/DomTokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/Token.h>

namespace Ludus::Tests::Persistance::Codecs
{
	using LmlDomCodec = Ludus::Engine::Persistance::Codecs::LmlDomCodec;
	using DomDocument = Ludus::Engine::Persistance::Serialization::DomDocument;
	using DomNode = Ludus::Engine::Persistance::Serialization::DomNode;
	using DomArray = Ludus::Engine::Persistance::Serialization::DomArray;
	using DomObject = Ludus::Engine::Persistance::Serialization::DomObject;
	using DomValue = Ludus::Engine::Persistance::Serialization::DomValue;
	using DomTokenStreamWriter = Ludus::Engine::Persistance::Serialization::DomTokenStreamWriter;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	using Ludus::Engine::Persistance::Serialization::AsArray;
	using Ludus::Engine::Persistance::Serialization::AsObject;
	using Ludus::Engine::Persistance::Serialization::AsValue;

#pragma region Helpers

	static std::string EncodeRootObject(std::function<void(DomTokenStreamWriter&)> build)
	{
		DomDocument document;
		DomTokenStreamWriter writer(document);

		writer.Emit(Token::StartObject {});
		build(writer);
		writer.Emit(Token::EndObject {});

		LmlDomCodec codec;
		return codec.Encode(*document.GetRoot());
	}

	static void EmitKey(DomTokenStreamWriter& w, const char* key)
	{
		w.Emit(Token::Key { key });
	}

	static const DomNode* FindMember(const DomObject& object, std::string_view key)
	{
		for (const auto& [memberKey, value] : object)
		{
			if (memberKey == key)
			{
				return value.get();
			}
		}
		return nullptr;
	}

	static void ExpectStringValue(const DomNode* node, std::string_view expected)
	{
		ASSERT_NE(node, nullptr);
		const auto* value = std::get_if<std::string>(&AsValue(*node));
		ASSERT_NE(value, nullptr);
		EXPECT_EQ(*value, expected);
	}

	static void ExpectBoolValue(const DomNode* node, bool expected)
	{
		ASSERT_NE(node, nullptr);
		const auto* value = std::get_if<bool>(&AsValue(*node));
		ASSERT_NE(value, nullptr);
		EXPECT_EQ(*value, expected);
	}

	static void ExpectNullValue(const DomNode* node)
	{
		ASSERT_NE(node, nullptr);
		ASSERT_TRUE(std::holds_alternative<std::monostate>(AsValue(*node)));
	}

	static void ExpectIntLikeValue(const DomNode* node, int expected)
	{
		ASSERT_NE(node, nullptr);
		const auto& value = AsValue(*node);
		if (const auto* intValue = std::get_if<int>(&value))
		{
			EXPECT_EQ(*intValue, expected);
			return;
		}
		if (const auto* uintValue = std::get_if<uint32_t>(&value))
		{
			EXPECT_EQ(*uintValue, static_cast<uint32_t>(expected));
			return;
		}
		FAIL() << "Expected int or uint32 value.";
	}

	static void ExpectFloatLikeValue(const DomNode* node, double expected)
	{
		ASSERT_NE(node, nullptr);
		const auto& value = AsValue(*node);
		if (const auto* floatValue = std::get_if<float>(&value))
		{
			EXPECT_FLOAT_EQ(*floatValue, static_cast<float>(expected));
			return;
		}
		if (const auto* doubleValue = std::get_if<double>(&value))
		{
			EXPECT_DOUBLE_EQ(*doubleValue, expected);
			return;
		}
		FAIL() << "Expected float or double value.";
	}

#pragma endregion

#pragma region Encode

	TEST(LmlDomCodec, Encode_WritesScalarValuesInOrder)
	{
		// Arrange.
		const auto expected = std::string("A: ValueA\nB: 2\n");

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "A");
				w.Emit(Token::String { "ValueA" });
				EmitKey(w, "B");
				w.Emit(Token::Int { 2 });
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_QuotesAndEscapesStrings)
	{
		// Arrange.
		const auto expected = std::string("Message: \"Hello:\\n\\\"World\\\"\\\\Test\"\n");

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Message");
				w.Emit(Token::String { "Hello:\n\"World\"\\Test" });
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_FormatsFloatsAndDoublesWithTwoDecimals)
	{
		// Arrange.
		const auto expected = std::string("FloatValue: 1.50\nDoubleValue: 2.00\n");

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "FloatValue");
				w.Emit(Token::Float { 1.5f });
				EmitKey(w, "DoubleValue");
				w.Emit(Token::Double { 2.0 });
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_EmptyObjectAndArray_AreInlineBracesAndBrackets)
	{
		// Arrange.
		const auto expected = std::string(
			"EmptyObject: {}\n"
			"EmptyArray: []\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "EmptyObject");
				w.Emit(Token::StartObject {});
				w.Emit(Token::EndObject {});

				EmitKey(w, "EmptyArray");
				w.Emit(Token::StartArray {});
				w.Emit(Token::EndArray {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_Object_Block_WhenNestedObject)
	{
		// Arrange.
		const auto expected = std::string(
			"Parent:\n"
			"  Child: Value\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Parent");
				w.Emit(Token::StartObject {});
				EmitKey(w, "Child");
				w.Emit(Token::String { "Value" });
				w.Emit(Token::EndObject {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_Object_Inline_WhenSmallScalarOnly)
	{
		// Arrange.
		const auto expected = std::string(
			"Transform: { X: 2.00, Y: 4.00 }\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Transform");
				w.Emit(Token::StartObject {});
				EmitKey(w, "X");
				w.Emit(Token::Float { 2.0f });
				EmitKey(w, "Y");
				w.Emit(Token::Float { 4.0f });
				w.Emit(Token::EndObject {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_Array_Inline_WhenSmallScalarArray)
	{
		// Arrange.
		const auto expected = std::string(
			"Ints: [1, 2, 3]\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Ints");
				w.Emit(Token::StartArray {});
				w.Emit(Token::Int { 1 });
				w.Emit(Token::Int { 2 });
				w.Emit(Token::Int { 3 });
				w.Emit(Token::EndArray {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_Array_Block_WhenElementsAreOnSeparateLogicalLines)
	{
		// Arrange.
		const auto expected = std::string(
			"Items:\n"
			"  - Handle: 1\n"
			"    Transform: { X: 2.00, Y: 4.00 }\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Items");
				w.Emit(Token::StartArray {});

				w.Emit(Token::StartObject {});
				EmitKey(w, "Handle");
				w.Emit(Token::Uint32 { 1u });
				EmitKey(w, "Transform");
				w.Emit(Token::StartObject {});
				EmitKey(w, "X");
				w.Emit(Token::Float { 2.0f });
				EmitKey(w, "Y");
				w.Emit(Token::Float { 4.0f });
				w.Emit(Token::EndObject {});
				w.Emit(Token::EndObject {});

				w.Emit(Token::EndArray {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_Array_Block_WhenNotInlineFriendly)
	{
		// Arrange.
		const auto expected = std::string(
			"Items:\n"
			"  - Child1: Value1\n"
			"  - Child2: Value2\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Items");
				w.Emit(Token::StartArray {});

				w.Emit(Token::StartObject {});
				EmitKey(w, "Child1");
				w.Emit(Token::String { "Value1" });
				w.Emit(Token::EndObject {});

				w.Emit(Token::StartObject {});
				EmitKey(w, "Child2");
				w.Emit(Token::String { "Value2" });
				w.Emit(Token::EndObject {});

				w.Emit(Token::EndArray {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_ArrayOfSingleKeyObjects_IsInlinePerElement)
	{
		// Arrange.
		const auto expected = std::string(
			"Items:\n"
			"  - Id: 1\n"
			"  - Id: 2\n"
		);

		// Act.
		const auto result = EncodeRootObject([](auto& w)
			{
				EmitKey(w, "Items");
				w.Emit(Token::StartArray {});

				w.Emit(Token::StartObject {});
				EmitKey(w, "Id");
				w.Emit(Token::Uint32 { 1u });
				w.Emit(Token::EndObject {});

				w.Emit(Token::StartObject {});
				EmitKey(w, "Id");
				w.Emit(Token::Uint32 { 2u });
				w.Emit(Token::EndObject {});

				w.Emit(Token::EndArray {});
			});

		// Assert.
		ASSERT_EQ(result, expected);
	}

	TEST(LmlDomCodec, Encode_Throws_WhenArrayHasMixedElementKinds)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Mixed" });
		writer.Emit(Token::StartArray {});
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "A" });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndArray {});
		writer.Emit(Token::EndObject {});

		// Act & Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Encode(*document.GetRoot()), std::runtime_error);
	}

	TEST(LmlDomCodec, Encode_TortureTest_ProducesExpectedYamlLikeOutput)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "TortureTest" });
		writer.Emit(Token::StartObject {});

		// Empty containers -> inline.
		writer.Emit(Token::Key { "EmptyObject" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::Key { "EmptyArray" });
		writer.Emit(Token::StartArray {});
		writer.Emit(Token::EndArray {});

		// Nested objects -> block.
		writer.Emit(Token::Key { "Scalars" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "BoolTrue" }); writer.Emit(Token::Bool { true });
		writer.Emit(Token::Key { "Int" });      writer.Emit(Token::Int { 42 });
		writer.Emit(Token::Key { "Float" });    writer.Emit(Token::Float { 1.5f });
		writer.Emit(Token::Key { "String" });   writer.Emit(Token::String { "Hello" });
		writer.Emit(Token::EndObject {});

		// Strings that can be ambiguous -> quote.
		writer.Emit(Token::Key { "Strings" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "WithColon" });        writer.Emit(Token::String { "a: b" });
		writer.Emit(Token::Key { "WithHash" });         writer.Emit(Token::String { "a # not comment" });
		writer.Emit(Token::Key { "WithLeadingDash" });  writer.Emit(Token::String { "- looks like list" });
		writer.Emit(Token::Key { "WithNewline" });      writer.Emit(Token::String { "Line1\nLine2" });
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::Key { "NullCases" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "ExplicitNull" }); writer.Emit(Token::Null {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::Key { "Arrays" });
		writer.Emit(Token::StartObject {});

		// Small scalar array -> inline [..].
		writer.Emit(Token::Key { "Ints" });
		writer.Emit(Token::StartArray {});
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::Int { 3 });
		writer.Emit(Token::EndArray {});

		// Array elements that are multi-line objects -> block array.
		writer.Emit(Token::Key { "ArrayOfObjects" });
		writer.Emit(Token::StartArray {});

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Handle" }); writer.Emit(Token::Uint32 { 1u });
		writer.Emit(Token::Key { "Transform" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "X" }); writer.Emit(Token::Float { 2.0f });
		writer.Emit(Token::Key { "Y" }); writer.Emit(Token::Float { 4.0f });
		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Handle" }); writer.Emit(Token::Uint32 { 2u });
		writer.Emit(Token::Key { "Transform" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "X" }); writer.Emit(Token::Float { -2.0f });
		writer.Emit(Token::Key { "Y" }); writer.Emit(Token::Float { -4.0f });
		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::EndArray {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::Key { "DiagnosticsTarget" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Array" });
		writer.Emit(Token::StartArray {});

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Obj" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "A" }); writer.Emit(Token::Int { 1 });
		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Obj" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "A" }); writer.Emit(Token::Int { 2 });
		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "Obj" });
		writer.Emit(Token::StartObject {});
		writer.Emit(Token::Key { "A" }); writer.Emit(Token::String { "wrong-type" });
		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::EndArray {});
		writer.Emit(Token::EndObject {});

		writer.Emit(Token::EndObject {});
		writer.Emit(Token::EndObject {});

		const auto expected = std::string(
			"TortureTest:\n"
			"  EmptyObject: {}\n"
			"  EmptyArray: []\n"
			"  Scalars:\n"
			"    BoolTrue: true\n"
			"    Int: 42\n"
			"    Float: 1.50\n"
			"    String: Hello\n"
			"  Strings:\n"
			"    WithColon: \"a: b\"\n"
			"    WithHash: \"a # not comment\"\n"
			"    WithLeadingDash: \"- looks like list\"\n"
			"    WithNewline: \"Line1\\nLine2\"\n"
			"  NullCases:\n"
			"    ExplicitNull: null\n"
			"  Arrays:\n"
			"    Ints: [1, 2, 3]\n"
			"    ArrayOfObjects:\n"
			"      - Handle: 1\n"
			"        Transform: { X: 2.00, Y: 4.00 }\n"
			"      - Handle: 2\n"
			"        Transform: { X: -2.00, Y: -4.00 }\n"
			"  DiagnosticsTarget:\n"
			"    Array:\n"
			"      - Obj: { A: 1 }\n"
			"      - Obj: { A: 2 }\n"
			"      - Obj: { A: \"wrong-type\" }\n"
		);

		// Act.
		LmlDomCodec codec;
		const auto result = codec.Encode(*document.GetRoot());

		// Assert.
		ASSERT_EQ(result, expected);
	}

#pragma endregion

#pragma region Decode

	TEST(LmlDomCodec, Decode_ReadsScalarValuesInOrder)
	{
		// Arrange.
		const auto input = std::string(
			"A: ValueA\n"
			"B: 2\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 2u);
		EXPECT_EQ(object[0].first, "A");
		ExpectStringValue(object[0].second.get(), "ValueA");
		EXPECT_EQ(object[1].first, "B");
		ExpectIntLikeValue(object[1].second.get(), 2);
	}

	TEST(LmlDomCodec, Decode_QuotesAndEscapesStrings)
	{
		// Arrange.
		const auto input = std::string(
			"Message: \"Hello:\\n\\\"World\\\"\\\\Test\"\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);
		EXPECT_EQ(object[0].first, "Message");
		ExpectStringValue(object[0].second.get(), "Hello:\n\"World\"\\Test");
	}

	TEST(LmlDomCodec, Decode_ParsesFloatsAndDoublesWithTwoDecimals)
	{
		// Arrange.
		const auto input = std::string(
			"FloatValue: 1.50\n"
			"DoubleValue: 2.00\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 2u);
		EXPECT_EQ(object[0].first, "FloatValue");
		ExpectFloatLikeValue(object[0].second.get(), 1.5);
		EXPECT_EQ(object[1].first, "DoubleValue");
		ExpectFloatLikeValue(object[1].second.get(), 2.0);
	}

	TEST(LmlDomCodec, Decode_EmptyObjectAndArray_AreInlineBracesAndBrackets)
	{
		// Arrange.
		const auto input = std::string(
			"EmptyObject: {}\n"
			"EmptyArray: []\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 2u);

		ASSERT_TRUE(std::holds_alternative<DomObject>(object[0].second->NodeData));
		EXPECT_TRUE(AsObject(*object[0].second).empty());

		ASSERT_TRUE(std::holds_alternative<DomArray>(object[1].second->NodeData));
		EXPECT_TRUE(AsArray(*object[1].second).empty());
	}

	TEST(LmlDomCodec, Decode_Object_Block_WhenNestedObject)
	{
		// Arrange.
		const auto input = std::string(
			"Parent:\n"
			"  Child: Value\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);
		EXPECT_EQ(object[0].first, "Parent");

		ASSERT_TRUE(std::holds_alternative<DomObject>(object[0].second->NodeData));
		const auto& parent = AsObject(*object[0].second);
		ASSERT_EQ(parent.size(), 1u);
		EXPECT_EQ(parent[0].first, "Child");
		ExpectStringValue(parent[0].second.get(), "Value");
	}

	TEST(LmlDomCodec, Decode_Object_Inline_WhenSmallScalarOnly)
	{
		// Arrange.
		const auto input = std::string(
			"Transform: { X: 2.00, Y: 4.00 }\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);
		EXPECT_EQ(object[0].first, "Transform");

		ASSERT_TRUE(std::holds_alternative<DomObject>(object[0].second->NodeData));
		const auto& transform = AsObject(*object[0].second);
		ASSERT_EQ(transform.size(), 2u);
		EXPECT_EQ(transform[0].first, "X");
		ExpectFloatLikeValue(transform[0].second.get(), 2.0);
		EXPECT_EQ(transform[1].first, "Y");
		ExpectFloatLikeValue(transform[1].second.get(), 4.0);
	}

	TEST(LmlDomCodec, Decode_Array_Inline_WhenSmallScalarArray)
	{
		// Arrange.
		const auto input = std::string(
			"Ints: [1, 2, 3]\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);
		EXPECT_EQ(object[0].first, "Ints");

		ASSERT_TRUE(std::holds_alternative<DomArray>(object[0].second->NodeData));
		const auto& array = AsArray(*object[0].second);
		ASSERT_EQ(array.size(), 3u);
		ExpectIntLikeValue(array[0].get(), 1);
		ExpectIntLikeValue(array[1].get(), 2);
		ExpectIntLikeValue(array[2].get(), 3);
	}

	TEST(LmlDomCodec, Decode_Array_Block_WhenElementsAreOnSeparateLogicalLines)
	{
		// Arrange.
		const auto input = std::string(
			"Items:\n"
			"  - Handle: 1\n"
			"    Transform: { X: 2.00, Y: 4.00 }\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);
		EXPECT_EQ(object[0].first, "Items");

		ASSERT_TRUE(std::holds_alternative<DomArray>(object[0].second->NodeData));
		const auto& array = AsArray(*object[0].second);
		ASSERT_EQ(array.size(), 1u);

		ASSERT_TRUE(std::holds_alternative<DomObject>(array[0]->NodeData));
		const auto& itemObject = AsObject(*array[0]);
		ASSERT_EQ(itemObject.size(), 2u);
		EXPECT_EQ(itemObject[0].first, "Handle");
		ExpectIntLikeValue(itemObject[0].second.get(), 1);
		EXPECT_EQ(itemObject[1].first, "Transform");

		ASSERT_TRUE(std::holds_alternative<DomObject>(itemObject[1].second->NodeData));
		const auto& transform = AsObject(*itemObject[1].second);
		ASSERT_EQ(transform.size(), 2u);
		EXPECT_EQ(transform[0].first, "X");
		ExpectFloatLikeValue(transform[0].second.get(), 2.0);
		EXPECT_EQ(transform[1].first, "Y");
		ExpectFloatLikeValue(transform[1].second.get(), 4.0);
	}

	TEST(LmlDomCodec, Decode_Array_Block_WhenNotInlineFriendly)
	{
		// Arrange.
		const auto input = std::string(
			"Items:\n"
			"  - Child1: Value1\n"
			"  - Child2: Value2\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);

		ASSERT_TRUE(std::holds_alternative<DomArray>(object[0].second->NodeData));
		const auto& array = AsArray(*object[0].second);
		ASSERT_EQ(array.size(), 2u);

		ASSERT_TRUE(std::holds_alternative<DomObject>(array[0]->NodeData));
		const auto& first = AsObject(*array[0]);
		ASSERT_EQ(first.size(), 1u);
		EXPECT_EQ(first[0].first, "Child1");
		ExpectStringValue(first[0].second.get(), "Value1");

		ASSERT_TRUE(std::holds_alternative<DomObject>(array[1]->NodeData));
		const auto& second = AsObject(*array[1]);
		ASSERT_EQ(second.size(), 1u);
		EXPECT_EQ(second[0].first, "Child2");
		ExpectStringValue(second[0].second.get(), "Value2");
	}

	TEST(LmlDomCodec, Decode_ArrayOfSingleKeyObjects_IsInlinePerElement)
	{
		// Arrange.
		const auto input = std::string(
			"Items:\n"
			"  - Id: 1\n"
			"  - Id: 2\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& object = AsObject(root);
		ASSERT_EQ(object.size(), 1u);

		ASSERT_TRUE(std::holds_alternative<DomArray>(object[0].second->NodeData));
		const auto& array = AsArray(*object[0].second);
		ASSERT_EQ(array.size(), 2u);

		ASSERT_TRUE(std::holds_alternative<DomObject>(array[0]->NodeData));
		const auto& first = AsObject(*array[0]);
		ASSERT_EQ(first.size(), 1u);
		EXPECT_EQ(first[0].first, "Id");
		ExpectIntLikeValue(first[0].second.get(), 1);

		ASSERT_TRUE(std::holds_alternative<DomObject>(array[1]->NodeData));
		const auto& second = AsObject(*array[1]);
		ASSERT_EQ(second.size(), 1u);
		EXPECT_EQ(second[0].first, "Id");
		ExpectIntLikeValue(second[0].second.get(), 2);
	}

	TEST(LmlDomCodec, Decode_TortureTest_ParsesExpectedDom)
	{
		// Arrange.
		const auto input = std::string(
			"TortureTest:\n"
			"  EmptyObject: {}\n"
			"  EmptyArray: []\n"
			"  Scalars:\n"
			"    BoolTrue: true\n"
			"    Int: 42\n"
			"    Float: 1.50\n"
			"    String: Hello\n"
			"  Strings:\n"
			"    WithColon: \"a: b\"\n"
			"    WithHash: \"a # not comment\"\n"
			"    WithLeadingDash: \"- looks like list\"\n"
			"    WithNewline: \"Line1\\nLine2\"\n"
			"  NullCases:\n"
			"    ExplicitNull: null\n"
			"  Arrays:\n"
			"    Ints: [1, 2, 3]\n"
			"    ArrayOfObjects:\n"
			"      - Handle: 1\n"
			"        Transform: { X: 2.00, Y: 4.00 }\n"
			"      - Handle: 2\n"
			"        Transform: { X: -2.00, Y: -4.00 }\n"
			"  DiagnosticsTarget:\n"
			"    Array:\n"
			"      - Obj: { A: 1 }\n"
			"      - Obj: { A: 2 }\n"
			"      - Obj: { A: \"wrong-type\" }\n"
		);

		// Act.
		LmlDomCodec codec;
		DomNode root = codec.Decode(input);

		// Assert.
		ASSERT_TRUE(std::holds_alternative<DomObject>(root.NodeData));
		const auto& rootObject = AsObject(root);
		ASSERT_EQ(rootObject.size(), 1u);
		EXPECT_EQ(rootObject[0].first, "TortureTest");

		ASSERT_TRUE(std::holds_alternative<DomObject>(rootObject[0].second->NodeData));
		const auto& torture = AsObject(*rootObject[0].second);

		const DomNode* emptyObject = FindMember(torture, "EmptyObject");
		ASSERT_NE(emptyObject, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(emptyObject->NodeData));
		EXPECT_TRUE(AsObject(*emptyObject).empty());

		const DomNode* emptyArray = FindMember(torture, "EmptyArray");
		ASSERT_NE(emptyArray, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomArray>(emptyArray->NodeData));
		EXPECT_TRUE(AsArray(*emptyArray).empty());

		const DomNode* scalarsNode = FindMember(torture, "Scalars");
		ASSERT_NE(scalarsNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(scalarsNode->NodeData));
		const auto& scalars = AsObject(*scalarsNode);
		ExpectBoolValue(FindMember(scalars, "BoolTrue"), true);
		ExpectIntLikeValue(FindMember(scalars, "Int"), 42);
		ExpectFloatLikeValue(FindMember(scalars, "Float"), 1.5);
		ExpectStringValue(FindMember(scalars, "String"), "Hello");

		const DomNode* stringsNode = FindMember(torture, "Strings");
		ASSERT_NE(stringsNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(stringsNode->NodeData));
		const auto& strings = AsObject(*stringsNode);
		ExpectStringValue(FindMember(strings, "WithColon"), "a: b");
		ExpectStringValue(FindMember(strings, "WithHash"), "a # not comment");
		ExpectStringValue(FindMember(strings, "WithLeadingDash"), "- looks like list");
		ExpectStringValue(FindMember(strings, "WithNewline"), "Line1\nLine2");

		const DomNode* nullCasesNode = FindMember(torture, "NullCases");
		ASSERT_NE(nullCasesNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(nullCasesNode->NodeData));
		const auto& nullCases = AsObject(*nullCasesNode);
		ExpectNullValue(FindMember(nullCases, "ExplicitNull"));

		const DomNode* arraysNode = FindMember(torture, "Arrays");
		ASSERT_NE(arraysNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(arraysNode->NodeData));
		const auto& arrays = AsObject(*arraysNode);

		const DomNode* intsNode = FindMember(arrays, "Ints");
		ASSERT_NE(intsNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomArray>(intsNode->NodeData));
		const auto& ints = AsArray(*intsNode);
		ASSERT_EQ(ints.size(), 3u);
		ExpectIntLikeValue(ints[0].get(), 1);
		ExpectIntLikeValue(ints[1].get(), 2);
		ExpectIntLikeValue(ints[2].get(), 3);

		const DomNode* arrayOfObjectsNode = FindMember(arrays, "ArrayOfObjects");
		ASSERT_NE(arrayOfObjectsNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomArray>(arrayOfObjectsNode->NodeData));
		const auto& arrayOfObjects = AsArray(*arrayOfObjectsNode);
		ASSERT_EQ(arrayOfObjects.size(), 2u);

		const auto& firstItem = AsObject(*arrayOfObjects[0]);
		ExpectIntLikeValue(FindMember(firstItem, "Handle"), 1);
		const DomNode* firstTransformNode = FindMember(firstItem, "Transform");
		ASSERT_NE(firstTransformNode, nullptr);
		const auto& firstTransform = AsObject(*firstTransformNode);
		ExpectFloatLikeValue(FindMember(firstTransform, "X"), 2.0);
		ExpectFloatLikeValue(FindMember(firstTransform, "Y"), 4.0);

		const auto& secondItem = AsObject(*arrayOfObjects[1]);
		ExpectIntLikeValue(FindMember(secondItem, "Handle"), 2);
		const DomNode* secondTransformNode = FindMember(secondItem, "Transform");
		ASSERT_NE(secondTransformNode, nullptr);
		const auto& secondTransform = AsObject(*secondTransformNode);
		ExpectFloatLikeValue(FindMember(secondTransform, "X"), -2.0);
		ExpectFloatLikeValue(FindMember(secondTransform, "Y"), -4.0);

		const DomNode* diagnosticsNode = FindMember(torture, "DiagnosticsTarget");
		ASSERT_NE(diagnosticsNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomObject>(diagnosticsNode->NodeData));
		const auto& diagnostics = AsObject(*diagnosticsNode);

		const DomNode* diagnosticsArrayNode = FindMember(diagnostics, "Array");
		ASSERT_NE(diagnosticsArrayNode, nullptr);
		ASSERT_TRUE(std::holds_alternative<DomArray>(diagnosticsArrayNode->NodeData));
		const auto& diagnosticsArray = AsArray(*diagnosticsArrayNode);
		ASSERT_EQ(diagnosticsArray.size(), 3u);

		const auto& diag0 = AsObject(*diagnosticsArray[0]);
		const DomNode* diag0ObjNode = FindMember(diag0, "Obj");
		ASSERT_NE(diag0ObjNode, nullptr);
		const auto& diag0Obj = AsObject(*diag0ObjNode);
		ExpectIntLikeValue(FindMember(diag0Obj, "A"), 1);

		const auto& diag1 = AsObject(*diagnosticsArray[1]);
		const DomNode* diag1ObjNode = FindMember(diag1, "Obj");
		ASSERT_NE(diag1ObjNode, nullptr);
		const auto& diag1Obj = AsObject(*diag1ObjNode);
		ExpectIntLikeValue(FindMember(diag1Obj, "A"), 2);

		const auto& diag2 = AsObject(*diagnosticsArray[2]);
		const DomNode* diag2ObjNode = FindMember(diag2, "Obj");
		ASSERT_NE(diag2ObjNode, nullptr);
		const auto& diag2Obj = AsObject(*diag2ObjNode);
		ExpectStringValue(FindMember(diag2Obj, "A"), "wrong-type");
	}

	TEST(LmlDomCodec, Decode_Throws_When_IndentationIsNotMultipleOfTwo)
	{
		// Arrange.
		const auto input = std::string(
			"Root:\n"
			"   Child: 1\n"
		);

		// Act + Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Decode(input), std::runtime_error);
	}

	TEST(LmlDomCodec, Decode_Throws_When_KeyHasNoValueAndNoBlock)
	{
		// Arrange.
		const auto input = std::string(
			"Root:\n"
			"  Child:\n"
			"  Other: 1\n"
		);

		// Act + Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Decode(input), std::runtime_error);
	}

	TEST(LmlDomCodec, Decode_Throws_When_QuotedStringIsUnterminated)
	{
		// Arrange.
		const auto input = std::string(
			"Message: \"unterminated\n"
		);

		// Act + Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Decode(input), std::runtime_error);
	}

	TEST(LmlDomCodec, Decode_Throws_When_InlineArrayHasTrailingComma)
	{
		// Arrange.
		const auto input = std::string(
			"Values: [1, 2,]\n"
		);

		// Act + Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Decode(input), std::runtime_error);
	}

	TEST(LmlDomCodec, Decode_Throws_When_InlineObjectHasTrailingComma)
	{
		// Arrange.
		const auto input = std::string(
			"Obj: { A: 1, }\n"
		);

		// Act + Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Decode(input), std::runtime_error);
	}

	TEST(LmlDomCodec, Decode_Throws_When_ArrayHasMixedElementKinds)
	{
		// Arrange.
		const auto input = std::string(
			"Mixed: [1, { A: 2 }]\n"
		);

		// Act + Assert.
		LmlDomCodec codec;
		ASSERT_THROW(codec.Decode(input), std::runtime_error);
	}

#pragma endregion

}
