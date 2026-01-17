#include "pch.h"

#include <cstdint>
#include <string>
#include <variant>

#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>

namespace Ludus::Tests::Persistance::Archives
{
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;

	using Ludus::Engine::Persistance::Archives::AsObject;
	using Ludus::Engine::Persistance::Archives::AsArray;
	using Ludus::Engine::Persistance::Archives::AsValue;

	static constexpr auto Object = "Object";
	static constexpr auto NestedObject = "NestedObject";
	static constexpr auto ArrayObject = "ArrayObject";
	static constexpr auto Key = "Key";

	TEST(DomSaveArchive, BeginObject_CreatesNestedObject_When_CalledInsideObject)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act.
		archive.BeginObject(Object);

		archive.BeginObject(NestedObject);
		archive.EndObject();

		archive.EndObject();

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& objectNode = rootObject.at(Object);
		const auto& object = AsObject(objectNode);
		ASSERT_TRUE(object.contains(NestedObject));
	}

	TEST(DomSaveArchive, BeginObject_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.BeginObject(Object); },
			R"(BeginObject must be used on an object\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndObject_ReturnsToParent_When_NestedObjectEnds)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act.
		archive.BeginObject(Object);
		archive.BeginObject(NestedObject);

		archive.EndObject();

		// Assert.
		ASSERT_EQ(archive.GetStackSize(), 2);
	}

	TEST(DomSaveArchive, EndObject_Fails_When_CalledAtRoot)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndObject(); },
			R"(EndObject must not be called on the Root node\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndObject_Fails_When_CurrentIsArray)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndObject(); },
			R"(EndObject must be used on an object\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndObject_Fails_When_CurrentIsArrayElementObject)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);
		archive.BeginArrayElement();

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndObject(); },
			R"(EndObject must be used inside an object\.)"
		);
#endif

	}

	TEST(DomSaveArchive, BeginArray_CreatesArray_When_CalledInsideObject)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act.
		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		archive.EndArray();
		archive.EndObject();

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& array = AsArray(AsObject(rootObject.at(Object)).at(ArrayObject));
		ASSERT_EQ(array.size(), 0);
	}

	TEST(DomSaveArchive, EndArray_Fails_When_CurrentIsArrayElementObject)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);
		archive.BeginArrayElement();

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndArray(); },
			R"(EndArray must be used on an array\.)"
		);
#endif

	}

	TEST(DomSaveArchive, BeginArrayElement_AppendsObject_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act.
		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		archive.BeginArrayElement();
		archive.EndArrayElement();

		archive.EndArray();
		archive.EndObject();

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& array = AsArray(AsObject(rootObject.at(Object)).at(ArrayObject));
		ASSERT_EQ(array.size(), 1);
	}

	TEST(DomSaveArchive, Array_PreservesInsertionOrder_When_MultipleElementsAdded)
	{
		// Arrange.
		const auto first = "First";
		const auto second = "Second";

		DomSaveArchive archive;

		// Act.
		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		archive.BeginArrayElement();
		archive.WriteString(Key, first);
		archive.EndArrayElement();

		archive.BeginArrayElement();
		archive.WriteString(Key, second);
		archive.EndArrayElement();

		archive.EndArray();
		archive.EndObject();

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& array = AsArray(AsObject(rootObject.at(Object)).at(ArrayObject));
		ASSERT_EQ(array.size(), 2);

		const auto& firstValueObject = AsObject(array[0]);
		const auto& secondValueObject = AsObject(array[1]);

		const auto& firstValue = AsValue(firstValueObject.at(Key));
		const auto& secondValue = AsValue(secondValueObject.at(Key));

		ASSERT_EQ(std::get<std::string>(firstValue), first);
		ASSERT_EQ(std::get<std::string>(secondValue), second);
	}

	TEST(DomSaveArchive, BeginArray_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.BeginArray(ArrayObject); },
			R"(BeginArray must be used on an object\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndArray_Fails_When_CalledAtRoot)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndArray(); },
			R"(EndArray must not be called on the Root node\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndArray_Fails_When_CurrentIsObject)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndArray(); },
			R"(EndArray must be used on an array\.)"
		);

#endif

	}

	TEST(DomSaveArchive, BeginArrayElement_Fails_When_CalledInsideObject)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.BeginArrayElement(); },
			R"(BeginArrayElement must be used on an array\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndArrayElement_Fails_When_CalledAtRoot)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndArrayElement(); },
			R"(EndArrayElement must not be called on the Root node\.)"
		);

#endif

	}

	TEST(DomSaveArchive, EndArrayElement_Fails_When_CurrentIsArray)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.EndArrayElement(); },
			R"(EndArrayElement must be used on an array element object\.)"
		);

#endif

	}

	TEST(DomSaveArchive, Write_PrimitivesStored_When_InObjectScope)
	{
		// Arrange.
		DomSaveArchive archive;

		// Act.
		archive.BeginObject(Object);

		archive.WriteBool("Bool", true);
		archive.WriteDouble("Double", 10.0);
		archive.WriteFloat("Float", 10.0f);
		archive.WriteInt("Int", 10);
		archive.WriteString("String", "10");
		archive.WriteUint8("Uint8", 10);
		archive.WriteUint32("Uint32", 10);

		archive.EndObject();

		// Assert.

		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& objectNode = rootObject.at(Object);
		const auto& object = AsObject(objectNode);

		const auto& boolValueNode = AsValue(object.at("Bool"));
		const auto& doubleValueNode = AsValue(object.at("Double"));
		const auto& floatValueNode = AsValue(object.at("Float"));
		const auto& intValueNode = AsValue(object.at("Int"));
		const auto& stringValueNode = AsValue(object.at("String"));
		const auto& uint8ValueNode = AsValue(object.at("Uint8"));
		const auto& uint32ValueNode = AsValue(object.at("Uint32"));

		ASSERT_EQ(std::get<bool>(boolValueNode), true);
		ASSERT_EQ(std::get<double>(doubleValueNode), 10.0);
		ASSERT_EQ(std::get<float>(floatValueNode), 10.0f);
		ASSERT_EQ(std::get<int>(intValueNode), 10);
		ASSERT_EQ(std::get<std::string>(stringValueNode), "10");
		ASSERT_EQ(std::get<uint8_t>(uint8ValueNode), 10);
		ASSERT_EQ(std::get<uint32_t>(uint32ValueNode), 10);
	}

	TEST(DomSaveArchive, Write_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive archive;

		archive.BeginObject(Object);
		archive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ archive.WriteBool(Key, true); },
			R"(Write must be used on an object\.)"
		);

#endif

	}
}
