#include "pch.h"

#include <string>
#include <variant>

#include <Ludus/Tests/Persistance/TestSaveArchive.h>

namespace Ludus::Tests::Persistance
{
	TEST(SaveArchive, Write_PrimitivesStored_When_InObjectScope)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");

		archive.WriteBool("Bool", true);
		archive.WriteDouble("Double", 10.0);
		archive.WriteFloat("Float", 10.0f);
		archive.WriteInt("Int", 10);
		archive.WriteString("String", "10");

		archive.EndObject();

		// Assert.
		const auto& object = archive.GetRoot().Object.at("Object");
		ASSERT_EQ(std::get<bool>(object.Object.at("Bool").Value), true);
		ASSERT_EQ(std::get<double>(object.Object.at("Double").Value), 10.0);
		ASSERT_EQ(std::get<float>(object.Object.at("Float").Value), 10.0f);
		ASSERT_EQ(std::get<int>(object.Object.at("Int").Value), 10);
		ASSERT_EQ(std::get<std::string>(object.Object.at("String").Value), "10");
	}

	TEST(SaveArchive, BeginObject_CreatesNestedObject_When_CalledInsideObject)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");
		archive.BeginObject("NestedObject");

		archive.EndObject();
		archive.EndObject();

		// Assert.
		const auto& object = archive.GetRoot().Object.at("Object");
		ASSERT_TRUE(object.Object.contains("NestedObject"));
	}

	TEST(SaveArchive, EndObject_ReturnsToParent_When_NestedObjectEnds)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");
		archive.BeginObject("NestedObject");

		archive.EndObject();

		// Assert.
		ASSERT_EQ(archive.GetStackSize(), 2);
	}

	TEST(SaveArchive, BeginArray_CreatesArray_When_CalledInsideObject)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");
		archive.BeginArray("ArrayObject");

		archive.EndArray();
		archive.EndObject();

		// Assert.
		const auto& object = archive.GetRoot().Object.at("Object");
		ASSERT_EQ(object.Object.at("ArrayObject").Array.size(), 0);
	}

	TEST(SaveArchive, BeginArrayElement_AppendsObject_When_CalledInsideArray)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");
		archive.BeginArray("ArrayObject");

		archive.BeginArrayElement();
		archive.EndArrayElement();

		archive.EndArray();
		archive.EndObject();

		// Assert.
		const auto& object = archive.GetRoot().Object.at("Object");
		const auto& array = object.Object.at("ArrayObject");
		ASSERT_EQ(array.Array.size(), 1);
	}

	TEST(SaveArchive, Array_PreservesInsertionOrder_When_MultipleElementsAdded)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");
		archive.BeginArray("ArrayObject");

		archive.BeginArrayElement();
		archive.WriteString("ArrayElementString", "First");
		archive.EndArrayElement();

		archive.BeginArrayElement();
		archive.WriteString("ArrayElementString", "Second");
		archive.EndArrayElement();

		archive.EndArray();
		archive.EndObject();

		// Assert.
		const auto& object = archive.GetRoot().Object.at("Object");
		const auto& array = object.Object.at("ArrayObject").Array;
		ASSERT_EQ(array.size(), 2);
		ASSERT_EQ(std::get<std::string>(array[0].Object.at("ArrayElementString").Value), "First");
		ASSERT_EQ(std::get<std::string>(array[1].Object.at("ArrayElementString").Value), "Second");
	}

	TEST(SaveArchive, Write_Fails_When_CalledInsideArray)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act.
		archive.BeginObject("Object");
		archive.BeginArray("ArrayObject");

		// Assert.
#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.WriteBool("Bool", true); },
			R"(Write must be inside object\.)"
		);

#endif
	}

	TEST(SaveArchive, EndObject_Fails_When_CalledAtRoot)
	{
		// Arrange.
		TestSaveArchive archive;

		// Act & Assert.
#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndObject(); },
			R"(EndObject must not be called on the Root node\.)"
		);

#endif
	}
}
