#include "pch.h"

#include <Ludus/Tests/Persistance/TestLoadArchive.h>
#include <Ludus/Tests/Persistance/TestSaveArchive.h>

namespace Ludus::Tests::Persistance
{
	TEST(LoadArchive, Has_ReturnsTrue_When_KeyExistsInObject)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");
		saveArchive.WriteString("String", "10");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");
		auto hasKey = loadArchive.Has("String");

		// Assert.
		ASSERT_TRUE(hasKey);
	}

	TEST(LoadArchive, Has_ReturnsFalse_When_KeyDoesNotExist)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");
		saveArchive.WriteString("String", "10");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");
		auto hasKey = loadArchive.Has("NotString");

		// Assert.
		ASSERT_FALSE(hasKey);
	}

	TEST(LoadArchive, TryRead_ReturnsTrueAndValue_When_KeyExistsWithCorrectType)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");

		saveArchive.WriteBool("Bool", true);
		saveArchive.WriteDouble("Double", 10.0);
		saveArchive.WriteFloat("Float", 10.0f);
		saveArchive.WriteInt("Int", 10);
		saveArchive.WriteString("String", "10");

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");

		bool boolValue;
		double doubleValue;
		float floatValue;
		int intValue;
		std::string stringValue;

		auto successBool = loadArchive.TryReadBool("Bool", boolValue);
		auto successDouble = loadArchive.TryReadDouble("Double", doubleValue);
		auto successFloat = loadArchive.TryReadFloat("Float", floatValue);
		auto successInt = loadArchive.TryReadInt("Int", intValue);
		auto successString = loadArchive.TryReadString("String", stringValue);

		// Assert.
		ASSERT_TRUE(successBool);
		ASSERT_TRUE(successDouble);
		ASSERT_TRUE(successFloat);
		ASSERT_TRUE(successInt);
		ASSERT_TRUE(successString);

		ASSERT_EQ(boolValue, true);
		ASSERT_EQ(doubleValue, 10.0);
		ASSERT_EQ(floatValue, 10.0f);
		ASSERT_EQ(intValue, 10);
		ASSERT_EQ(stringValue, "10");
	}

	TEST(LoadArchive, TryRead_ReturnsFalse_When_KeyExistsWithWrongType)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");

		saveArchive.WriteBool("WrongDouble", true);
		saveArchive.WriteDouble("WrongFloat", 10.0);
		saveArchive.WriteFloat("WrongInt", 10.0f);
		saveArchive.WriteInt("WrongString", 10);
		saveArchive.WriteString("WrongBool", "10");

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");

		bool boolValue;
		double doubleValue;
		float floatValue;
		int intValue;
		std::string stringValue;

		auto successBool = loadArchive.TryReadBool("WrongBool", boolValue);
		auto successDouble = loadArchive.TryReadDouble("WrongDouble", doubleValue);
		auto successFloat = loadArchive.TryReadFloat("WrongFloat", floatValue);
		auto successInt = loadArchive.TryReadInt("WrongInt", intValue);
		auto successString = loadArchive.TryReadString("WrongString", stringValue);

		// Assert.
		ASSERT_FALSE(successBool);
		ASSERT_FALSE(successDouble);
		ASSERT_FALSE(successFloat);
		ASSERT_FALSE(successInt);
		ASSERT_FALSE(successString);
	}

	TEST(LoadArchive, TryRead_ReturnsFalse_When_KeyDoesNotExist)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");

		saveArchive.WriteBool("Bool", true);
		saveArchive.WriteDouble("Double", 10.0);
		saveArchive.WriteFloat("Float", 10.0f);
		saveArchive.WriteInt("Int", 10);
		saveArchive.WriteString("String", "10");

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");

		bool boolValue;
		double doubleValue;
		float floatValue;
		int intValue;
		std::string stringValue;

		auto successBool = loadArchive.TryReadBool("IncorrectKey", boolValue);
		auto successDouble = loadArchive.TryReadDouble("IncorrectKey", doubleValue);
		auto successFloat = loadArchive.TryReadFloat("IncorrectKey", floatValue);
		auto successInt = loadArchive.TryReadInt("IncorrectKey", intValue);
		auto successString = loadArchive.TryReadString("IncorrectKey", stringValue);

		// Assert.
		ASSERT_FALSE(successBool);
		ASSERT_FALSE(successDouble);
		ASSERT_FALSE(successFloat);
		ASSERT_FALSE(successInt);
		ASSERT_FALSE(successString);
	}

	TEST(LoadArchive, BeginObject_EntersNestedObject_When_KeyExists)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");

		saveArchive.BeginObject("NestedObject");
		saveArchive.WriteString("Key", "StringValue");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");
		auto successNestedObject = loadArchive.Has("NestedObject");

		loadArchive.BeginObject("NestedObject");

		std::string stringValue;
		auto successString = loadArchive.TryReadString("Key", stringValue);

		ASSERT_TRUE(successNestedObject);
		ASSERT_TRUE(successString);

		ASSERT_EQ(stringValue, "StringValue");
	}

	TEST(LoadArchive, BeginArray_ReturnsCorrectSize_When_ArrayExists)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");

		saveArchive.BeginArray("ArrayObject");

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");

		auto arraySize = loadArchive.BeginArray("ArrayObject");

		// Assert.
		ASSERT_EQ(arraySize, 3);
	}

	TEST(LoadArchive, BeginArrayElement_IteratesElementsInOrder_When_ArrayTraversed)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");

		saveArchive.BeginArray("ArrayObject");

		saveArchive.BeginArrayElement();
		saveArchive.WriteString("Key1", "StringValue");
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteString("Key2", "StringValue");
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteString("Key3", "StringValue");
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject("Object");

		auto arraySize = loadArchive.BeginArray("ArrayObject");

		loadArchive.BeginArrayElement(0);
		auto successArray1 = loadArchive.Has("Key1");
		loadArchive.EndArrayElement();

		loadArchive.BeginArrayElement(1);
		auto successArray2 = loadArchive.Has("Key2");
		loadArchive.EndArrayElement();

		loadArchive.BeginArrayElement(2);
		auto successArray3 = loadArchive.Has("Key3");
		loadArchive.EndArrayElement();

		// Assert.
		ASSERT_TRUE(successArray1);
		ASSERT_TRUE(successArray2);
		ASSERT_TRUE(successArray3);
	}

	TEST(LoadArchive, BeginArray_Fails_When_KeyIsNotArray)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");
		saveArchive.WriteString("NotArrayObject", "I am not an array.");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		loadArchive.BeginObject("Object");

		// Act & Assert.
#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.BeginArray("NotArrayObject"); },
			R"(BeginArray must be used on an object containing an array\.)"
		);

#endif
	}

	TEST(LoadArchive, BeginArray_Fails_When_KeyDoesNotExist)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		loadArchive.BeginObject("Object");

		// Act & Assert.
#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.BeginArray("ArrayObject"); },
			R"(BeginArray must use a valid key\.)"
		);

#endif
	}

	TEST(LoadArchive, EndObject_Fails_When_CalledAtRoot)
	{
		// Arrange.
		TestSaveArchive saveArchive;

		saveArchive.BeginObject("Object");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		TestLoadArchive loadArchive(root);

		// Act & Assert.
#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.EndObject(); },
			R"(EndObject must not be called on the Root node\.)"
		);

#endif
	}
}
