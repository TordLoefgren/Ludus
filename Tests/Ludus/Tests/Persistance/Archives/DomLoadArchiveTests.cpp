#include "pch.h"

#include <Ludus/Engine/Persistance/Archives/ArchiveAction.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>

namespace Ludus::Tests::Persistance::Archives
{
	using ArchiveAction = Ludus::Engine::Persistance::Archives::ArchiveAction;
	using ArchiveSeverity = Ludus::Engine::Persistance::Archives::ArchiveSeverity;
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;

	static constexpr auto Object = "Object";
	static constexpr auto NestedObject = "NestedObject";
	static constexpr auto ArrayObject = "ArrayObject";
	static constexpr auto Key = "Key";
	static constexpr auto MissingKey = "MissingKey";
	static constexpr auto StringValue = "StringValue";
	static constexpr auto DefaultStringValue = "DefaultStringValue";

	TEST(DomLoadArchive, TryBeginObject_ReturnsTrueAndEntersNestedObject_When_KeyExists)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginObject(NestedObject);
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);
		const auto successNestedObject = loadArchive.Has(NestedObject);
		const auto beginObjectSuccess = loadArchive.TryBeginObject(NestedObject);

		// Assert.
		ASSERT_TRUE(successNestedObject);
		ASSERT_TRUE(beginObjectSuccess);
	}

	TEST(DomLoadArchive, TryBeginObject_ReturnsFalse_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginObject(NestedObject);
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);
		const auto successNestedObject = loadArchive.Has(MissingKey);
		const auto beginObjectSuccess = loadArchive.TryBeginObject(MissingKey);

		// Assert.
		ASSERT_FALSE(successNestedObject);
		ASSERT_FALSE(beginObjectSuccess);
	}

	TEST(DomLoadArchive, TryBeginObject_ReportsWarning_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginObject(NestedObject);
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		loadArchive.BeginObject(Object);
		const auto success = loadArchive.TryBeginObject(MissingKey);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid key: ") + MissingKey);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + MissingKey);
	}

	TEST(DomLoadArchive, TryBeginObject_ReturnsFalse_When_KeyIsNotObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		const auto success = loadArchive.TryBeginObject(ArrayObject);

		// Assert.
		ASSERT_FALSE(success);
	}

	TEST(DomLoadArchive, TryBeginObject_ReportsWarning_When_KeyIsNotObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.WriteString(Key, StringValue);

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto successArray = loadArchive.TryBeginObject(ArrayObject);
		const auto successValue = loadArchive.TryBeginObject(Key);

		// Assert.
		ASSERT_FALSE(successArray);
		ASSERT_FALSE(successValue);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 2);

		const auto& issueArray = issues[0];
		ASSERT_EQ(issueArray.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issueArray.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issueArray.Message, std::string("Key was not an object: ") + ArrayObject);
		ASSERT_EQ(issueArray.Path, std::string(ArrayObject));

		const auto& issueValue = issues[1];
		ASSERT_EQ(issueValue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issueValue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issueValue.Message, std::string("Key was not an object: ") + Key);
		ASSERT_EQ(issueValue.Path, std::string(Key));
	}

	TEST(DomLoadArchive, TryBeginObject_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);
		loadArchive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.TryBeginObject(ArrayObject); },
			R"(TryBeginObject must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, TryBeginObject_DoesNotChangeCursor_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);
		loadArchive.BeginObject(Object);

		const auto stackSizeBefore = loadArchive.GetStackSize();

		// Act.
		const auto success = loadArchive.TryBeginObject(MissingKey);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_EQ(loadArchive.GetStackSize(), stackSizeBefore);

		std::string out;
		ASSERT_TRUE(loadArchive.TryReadString(Key, out));
		ASSERT_EQ(out, StringValue);
	}

	TEST(DomLoadArchive, TryBeginObject_DoesNotChangeCursor_When_KeyIsNotObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();
		saveArchive.WriteString(Key, StringValue);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		const auto stackSizeBefore = loadArchive.GetStackSize();

		// Act.
		const auto success = loadArchive.TryBeginObject(ArrayObject);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_EQ(loadArchive.GetStackSize(), stackSizeBefore);

		std::string out;
		ASSERT_TRUE(loadArchive.TryReadString(Key, out));
		ASSERT_EQ(out, StringValue);
	}


	TEST(DomLoadArchive, BeginObject_EntersNestedObject_When_KeyExists)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginObject(NestedObject);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);
		auto successNestedObject = loadArchive.Has(NestedObject);

		loadArchive.BeginObject(NestedObject);

		std::string stringValue;
		auto successString = loadArchive.TryReadString(Key, stringValue);

		// Assert.
		ASSERT_TRUE(successNestedObject);
		ASSERT_TRUE(successString);
		ASSERT_EQ(stringValue, StringValue);
	}

	TEST(DomLoadArchive, BeginObject_Throws_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginObject(NestedObject);
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		EXPECT_THROW({ loadArchive.BeginObject(MissingKey); }, Ludus::Engine::Persistance::PersistanceException);
	}

	TEST(DomLoadArchive, BeginObject_Throws_When_KeyIsNotObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.WriteString(Key, StringValue);

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		EXPECT_THROW({ loadArchive.BeginObject(ArrayObject); }, Ludus::Engine::Persistance::PersistanceException);
		EXPECT_THROW({ loadArchive.BeginObject(Key); }, Ludus::Engine::Persistance::PersistanceException);
	}

	TEST(DomLoadArchive, BeginObject_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);
		loadArchive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.BeginObject(ArrayObject); },
			R"(BeginObject must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, EndObject_DecrementsStack_When_CalledAfterBeginObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);
		const auto stackSizeBefore = loadArchive.GetStackSize();

		// Act.
		loadArchive.EndObject();

		// Assert.
		ASSERT_LT(loadArchive.GetStackSize(), stackSizeBefore);
	}

	TEST(DomLoadArchive, EndObject_Fails_When_CalledAtRoot)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.EndObject(); },
			R"(EndObject must not be called on the Root node\.)"
		);

#endif

	}

	TEST(DomLoadArchive, TryBeginArray_ReturnsTrueAndOutputsCount_When_ArrayExists)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		size_t arraySize;
		const auto success = loadArchive.TryBeginArray(ArrayObject, arraySize);

		// Assert.
		ASSERT_TRUE(success);
		ASSERT_EQ(arraySize, 3);
	}

	TEST(DomLoadArchive, TryBeginArray_ReturnsFalse_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		size_t arraySize = 123;
		const auto success = loadArchive.TryBeginArray(MissingKey, arraySize);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_EQ(arraySize, 123);
	}

	TEST(DomLoadArchive, TryBeginArray_ReturnsFalse_When_KeyIsNotArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Key);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		size_t arraySize = 123;
		const auto success = loadArchive.TryBeginArray(Key, arraySize);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_EQ(arraySize, 123);
	}

	TEST(DomLoadArchive, TryBeginArray_ReportsWarning_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		size_t arraySize;
		const auto success = loadArchive.TryBeginArray(MissingKey, arraySize);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid key: ") + MissingKey);
		ASSERT_EQ(issue.Path, std::string(MissingKey));
	}

	TEST(DomLoadArchive, TryBeginArray_ReportsWarning_When_KeyIsNotArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		size_t arraySize;
		const auto success = loadArchive.TryBeginArray(Object, arraySize);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Key was not an array object: ") + Object);
		ASSERT_EQ(issue.Path, std::string(Object));
	}

	TEST(DomLoadArchive, TryBeginArray_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		size_t outCount;
		ASSERT_DEATH(
			{ loadArchive.TryBeginArray(ArrayObject, outCount); },
			R"(TryBeginArray must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, TryBeginArray_DoesNotChangeCursor_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.WriteString(Key, StringValue);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);
		const auto stackSizeBefore = loadArchive.GetStackSize();

		// Act.
		size_t arraySize;
		const auto success = loadArchive.TryBeginArray(MissingKey, arraySize);

		// Assert.
		ASSERT_FALSE(success);
		std::string out;
		ASSERT_TRUE(loadArchive.TryReadString(Key, out));
		ASSERT_EQ(out, StringValue);
	}

	TEST(DomLoadArchive, BeginArray_ReturnsCorrectSize_When_ArrayExists)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);

		auto arraySize = loadArchive.BeginArray(ArrayObject);

		// Assert.
		ASSERT_EQ(arraySize, 3);
	}

	TEST(DomLoadArchive, BeginArray_Fails_When_KeyIsNotArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		EXPECT_THROW({ loadArchive.BeginArray(Key); }, Ludus::Engine::Persistance::PersistanceException);
	}

	TEST(DomLoadArchive, BeginArray_Throws_When_KeyDoesNotExistInObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		EXPECT_THROW({ loadArchive.BeginArray(Key); }, Ludus::Engine::Persistance::PersistanceException);
	}

	TEST(DomLoadArchive, BeginArray_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);
		loadArchive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.BeginArray(ArrayObject); },
			R"(BeginArray must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, EndArray_DecrementsStack_When_CalledAfterBeginArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginArray(ArrayObject);
		const auto stackSizeBefore = loadArchive.GetStackSize();

		// Act.
		loadArchive.EndArray();

		// Assert.
		ASSERT_LT(loadArchive.GetStackSize(), stackSizeBefore);
	}

	TEST(DomLoadArchive, EndArray_Fails_When_CalledAtRoot)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.EndArray(); },
			R"(EndArray must not be called on the Root node\.)"
		);

#endif

	}

	TEST(DomLoadArchive, BeginArrayElement_IteratesElementsInOrder_When_ArrayTraversed)
	{
		// Arrange.

		const auto key1 = "Key1";
		const auto key2 = "Key2";
		const auto key3 = "Key3";

		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);

		saveArchive.BeginArrayElement();
		saveArchive.WriteString(key1, StringValue);
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteString(key2, StringValue);
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteString(key3, StringValue);
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);

		auto arraySize = loadArchive.BeginArray(ArrayObject);

		loadArchive.BeginArrayElement(0);
		auto successArray1 = loadArchive.Has(key1);
		loadArchive.EndArrayElement();

		loadArchive.BeginArrayElement(1);
		auto successArray2 = loadArchive.Has(key2);
		loadArchive.EndArrayElement();

		loadArchive.BeginArrayElement(2);
		auto successArray3 = loadArchive.Has(key3);
		loadArchive.EndArrayElement();

		// Assert.
		ASSERT_EQ(arraySize, 3);
		ASSERT_TRUE(successArray1);
		ASSERT_TRUE(successArray2);
		ASSERT_TRUE(successArray3);
	}

	TEST(DomLoadArchive, BeginArrayElement_Fails_When_CalledInsideObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.BeginArrayElement(0); },
			R"(BeginArrayElement must be inside array\.)"
		);

#endif

	}

	TEST(DomLoadArchive, BeginArrayElement_Fails_When_IndexOutOfRange)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginArray(ArrayObject);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.BeginArrayElement(1); },
			R"(BeginArrayElement must use a valid index\.)"
		);

#endif

	}

	TEST(DomLoadArchive, EndArrayElement_DecrementsStack_When_CalledAfterBeginArrayElement)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);

		saveArchive.BeginArrayElement();
		saveArchive.EndArrayElement();

		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginArray(ArrayObject);
		loadArchive.BeginArrayElement(0);
		const auto stackSizeBefore = loadArchive.GetStackSize();

		// Act.
		loadArchive.EndArrayElement();

		// Assert.
		ASSERT_LT(loadArchive.GetStackSize(), stackSizeBefore);
	}

	TEST(DomLoadArchive, EndArrayElement_Fails_When_CalledAtRoot)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act & Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.EndArrayElement(); },
			R"(EndArrayElement must not be called on the Root node\.)"
		);

#endif

	}

	TEST(DomLoadArchive, Has_ReturnsTrue_When_KeyExistsInObject)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);
		auto hasKey = loadArchive.Has(Key);

		// Assert.
		ASSERT_TRUE(hasKey);
	}

	TEST(DomLoadArchive, Has_ReturnsFalse_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);
		auto hasKey = loadArchive.Has(MissingKey);

		// Assert.
		ASSERT_FALSE(hasKey);
	}

	TEST(DomLoadArchive, Has_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginArray(ArrayObject);

		// Assert.

#if LUDUS_ENABLE_ASSERTS

		ASSERT_DEATH(
			{ loadArchive.Has(Key); },
			R"(Has must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, TryRead_ReturnsTrueAndValue_When_KeyExistsWithCorrectType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.WriteBool("Bool", true);
		saveArchive.WriteDouble("Double", 10.0);
		saveArchive.WriteFloat("Float", 10.0f);
		saveArchive.WriteInt("Int", 10);
		saveArchive.WriteString("String", "10");
		saveArchive.WriteUint32("Uint", 10);

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);

		bool boolValue;
		double doubleValue;
		float floatValue;
		int intValue;
		std::string stringValue;
		uint32_t uintValue;

		auto successBool = loadArchive.TryReadBool("Bool", boolValue);
		auto successDouble = loadArchive.TryReadDouble("Double", doubleValue);
		auto successFloat = loadArchive.TryReadFloat("Float", floatValue);
		auto successInt = loadArchive.TryReadInt("Int", intValue);
		auto successString = loadArchive.TryReadString("String", stringValue);
		auto successUint = loadArchive.TryReadUint32("Uint", uintValue);

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

	TEST(DomLoadArchive, TryRead_ReturnsFalse_When_KeyExistsWithWrongType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.WriteBool("Double", true);
		saveArchive.WriteDouble("Float", 10.0);
		saveArchive.WriteFloat("Int", 10.0f);
		saveArchive.WriteInt("String", 10);
		saveArchive.WriteString("Bool", "10");
		saveArchive.WriteUint32("Uint", 10);

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);

		bool boolValue;
		double doubleValue;
		float floatValue;
		int intValue;
		std::string stringValue;
		uint32_t uintValue;

		auto successBool = loadArchive.TryReadBool("Bool", boolValue);
		auto successDouble = loadArchive.TryReadDouble("Double", doubleValue);
		auto successFloat = loadArchive.TryReadFloat("Float", floatValue);
		auto successInt = loadArchive.TryReadInt("Int", intValue);
		auto successString = loadArchive.TryReadString("String", stringValue);
		auto successUint = loadArchive.TryReadUint32("Uint", uintValue);

		// Assert.
		ASSERT_FALSE(successBool);
		ASSERT_FALSE(successDouble);
		ASSERT_FALSE(successFloat);
		ASSERT_FALSE(successInt);
		ASSERT_FALSE(successString);
	}

	TEST(DomLoadArchive, TryRead_ReturnsFalse_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.WriteBool("Bool", true);
		saveArchive.WriteDouble("Double", 10.0);
		saveArchive.WriteFloat("Float", 10.0f);
		saveArchive.WriteInt("Int", 10);
		saveArchive.WriteString("String", "10");
		saveArchive.WriteUint32("Uint", 10);

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginObject(Object);

		bool boolValue;
		double doubleValue;
		float floatValue;
		int intValue;
		std::string stringValue;
		uint32_t uintValue;

		auto successBool = loadArchive.TryReadBool(MissingKey, boolValue);
		auto successDouble = loadArchive.TryReadDouble(MissingKey, doubleValue);
		auto successFloat = loadArchive.TryReadFloat(MissingKey, floatValue);
		auto successInt = loadArchive.TryReadInt(MissingKey, intValue);
		auto successString = loadArchive.TryReadString(MissingKey, stringValue);
		auto successUint = loadArchive.TryReadUint32(MissingKey, uintValue);

		// Assert.
		ASSERT_FALSE(successBool);
		ASSERT_FALSE(successDouble);
		ASSERT_FALSE(successFloat);
		ASSERT_FALSE(successInt);
		ASSERT_FALSE(successString);
	}

	TEST(DomLoadArchive, TryRead_ReportsWarning_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act.
		std::string outString;
		const auto success = loadArchive.TryReadString(MissingKey, outString);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid key: ") + MissingKey);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + std::string(MissingKey));
	}

	TEST(DomLoadArchive, TryRead_ReportsWarning_When_KeyExistsWithWrongType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act.
		bool outBool;
		const auto success = loadArchive.TryReadBool(Key, outBool);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid type for key: ") + Key);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + std::string(Key));
	}

	TEST(DomLoadArchive, TryRead_DoesNotReportWarning_When_KeyExistsWithCorrectType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act.
		std::string outString;
		const auto success = loadArchive.TryReadString(Key, outString);

		// Assert.
		ASSERT_TRUE(success);
		ASSERT_FALSE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 0);
	}

	TEST(DomLoadArchive, TryRead_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginArray(ArrayObject);

		// Assert.

#if LUDUS_ENABLE_ASSERTS

		std::string outString;
		ASSERT_DEATH(
			{ loadArchive.TryReadString(Key, outString); },
			R"(TryRead must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, TryRead_DoesNotModifyOutValue_When_Fails)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginArray(ArrayObject);

		// Assert.

#if LUDUS_ENABLE_ASSERTS

		int outInt = 123;
		ASSERT_DEATH(
			{ loadArchive.TryReadInt(MissingKey, outInt); },
			R"(TryRead must be used on an object\.)"
		);
		ASSERT_EQ(outInt, 123);

#endif

	}

	TEST(DomLoadArchive, TryRead_DoesNotModifyOutValue_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteInt(Key, 123);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		int outInt = 456;
		const auto success = loadArchive.TryReadInt(MissingKey, outInt);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_EQ(outInt, 456);
	}

	TEST(DomLoadArchive, TryRead_DoesNotModifyOutValue_When_KeyExistsWithWrongType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		int outInt = 123;
		const auto success = loadArchive.TryReadInt(Key, outInt);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_EQ(outInt, 123);
	}

	TEST(DomLoadArchive, Read_ReturnsValue_When_KeyExistsWithCorrectType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		std::string outString;
		loadArchive.ReadString(Key, outString);

		// Assert.
		ASSERT_EQ(outString, StringValue);
	}

	TEST(DomLoadArchive, Read_Throws_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		std::string outString;
		ASSERT_THROW({ loadArchive.ReadString(MissingKey, outString); }, Ludus::Engine::Persistance::PersistanceException);
	}

	TEST(DomLoadArchive, Read_Throws_When_KeyExistsWithWrongType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		int outInt = 456;
		ASSERT_THROW({ loadArchive.ReadInt(Key, outInt); }, Ludus::Engine::Persistance::PersistanceException);
	}

	TEST(DomLoadArchive, Read_Fails_When_CalledInsideArray)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginArray(ArrayObject);
		saveArchive.EndArray();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		loadArchive.BeginArray(ArrayObject);

		// Assert.

#if LUDUS_ENABLE_ASSERTS

		std::string outString;
		ASSERT_DEATH(
			{ loadArchive.ReadString(Key, outString); },
			R"(Read must be used on an object\.)"
		);

#endif

	}

	TEST(DomLoadArchive, Read_Throws_DoesNotModifyOutValue)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act & Assert.
		int outInt = 123;
		ASSERT_THROW({ loadArchive.ReadInt(Key, outInt); }, Ludus::Engine::Persistance::PersistanceException);
		ASSERT_EQ(outInt, 123);
	}

	TEST(DomLoadArchive, ReadOrDefault_SetsDefault_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		std::string outString;
		loadArchive.ReadStringOrDefault(MissingKey, outString, DefaultStringValue);

		// Assert.
		ASSERT_EQ(outString, DefaultStringValue);
	}

	TEST(DomLoadArchive, ReadOrDefault_SetsDefault_When_KeyExistsWithWrongType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		int outInt;
		loadArchive.ReadIntOrDefault(Key, outInt, 123);

		// Assert.
		ASSERT_EQ(outInt, 123);
	}

	TEST(DomLoadArchive, ReadOrDefault_ReadsValue_When_KeyExistsWithCorrectType)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		loadArchive.BeginObject(Object);

		// Act.
		std::string outString;
		loadArchive.ReadStringOrDefault(Key, outString, DefaultStringValue);

		// Assert.
		ASSERT_EQ(outString, StringValue);
	}

	TEST(DomLoadArchive, ReadOrDefault_ReportsWarning_When_DefaultUsed)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act.
		int outInt;
		loadArchive.ReadIntOrDefault(Key, outInt, 123);

		// Assert.
		ASSERT_EQ(outInt, 123);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid type for key: ") + Key);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + std::string(Key));
	}

	TEST(DomLoadArchive, ReadOrDefault_SetsDefaultAndReportsWarning_When_KeyDoesNotExist)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);

		// Act.
		std::string outString;
		loadArchive.ReadStringOrDefault(MissingKey, outString, DefaultStringValue);

		// Assert.
		ASSERT_EQ(outString, DefaultStringValue);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid key: ") + MissingKey);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + std::string(MissingKey));
	}

	TEST(DomLoadArchive, TryRead_ReportsWarning_WithFullPath_InNestedObject_When_KeyMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);
		saveArchive.BeginObject(NestedObject);
		saveArchive.WriteString(Key, StringValue);
		saveArchive.EndObject();
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);
		loadArchive.BeginObject(NestedObject);

		// Act.
		std::string out;
		const auto success = loadArchive.TryReadString(MissingKey, out);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid key: ") + MissingKey);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + std::string(NestedObject) + "." + std::string(MissingKey));
	}

	TEST(DomLoadArchive, TryRead_ReportsWarning_WithFullPath_InArrayElement_When_KeyMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;

		saveArchive.BeginObject(Object);

		saveArchive.BeginArray(ArrayObject);

		saveArchive.BeginArrayElement();
		saveArchive.WriteString(Key, "A");
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteString(Key, "B");
		saveArchive.EndArrayElement();

		saveArchive.BeginArrayElement();
		saveArchive.WriteString(Key, "C");
		saveArchive.EndArrayElement();

		saveArchive.EndArray();
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		loadArchive.BeginObject(Object);
		loadArchive.BeginArray(ArrayObject);
		loadArchive.BeginArrayElement(2);

		// Act.
		std::string out;
		const auto success = loadArchive.TryReadString(MissingKey, out);

		// Assert.
		ASSERT_FALSE(success);
		ASSERT_TRUE(diagnostics.HasWarnings());

		const auto& issues = diagnostics.GetIssues();
		ASSERT_EQ(issues.size(), 1);

		const auto& issue = issues[0];
		ASSERT_EQ(issue.Severity, ArchiveSeverity::Warn);
		ASSERT_EQ(issue.Action, ArchiveAction::ReadFailed);
		ASSERT_EQ(issue.Message, std::string("Object did not contain a valid key: ") + MissingKey);
		ASSERT_EQ(issue.Path, std::string(Object) + "." + std::string(ArrayObject) + "[2]." + std::string(MissingKey));
	}
}
