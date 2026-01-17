#include "pch.h"

#include <string>

#include <Ludus/Engine/Core/Expected.h>

namespace Ludus::Tests::Core
{
	using Ludus::Engine::Core::Expected;
	using Ludus::Engine::Core::Unexpected;

	TEST(Expected, ConstructFromValue_SetsHasValueTrue)
	{
		// Arrange.
		auto expected = Expected<std::string, std::string>("OK");

		// Act.
		auto hasValue = expected.HasValue();

		// Assert.
		ASSERT_TRUE(hasValue);
	}

	TEST(Expected, ConstructFromValue_StoresValue)
	{
		// Arrange.
		auto expected = Expected<std::string, std::string>("OK");

		// Act.
		const auto& value = expected.GetValue();

		// Assert.
		ASSERT_EQ(value, "OK");
	}

	TEST(Expected, ConstructFromUnexpected_SetsHasValueFalse)
	{
		// Arrange.
		auto expected = Expected<std::string, std::string>(Unexpected<std::string>("FAIL"));

		// Act.
		auto hasValue = expected.HasValue();

		// Assert.
		ASSERT_FALSE(hasValue);
	}

	TEST(Expected, ConstructFromUnexpected_StoresError)
	{
		// Arrange.
		auto expected = Expected<std::string, std::string>(Unexpected<std::string>("FAIL"));

		// Act.
		const auto& error = expected.GetError();

		// Assert.
		ASSERT_EQ(error, "FAIL");
	}

	TEST(Expected, ConstructFromValue_RvalueCompilesAndWorks)
	{
		// Arrange.
		auto expected = Expected<std::string, std::string>(std::string("OK"));

		// Act.
		const auto& value = expected.GetValue();

		// Assert.
		ASSERT_EQ(value, "OK");
	}

	TEST(Expected, ConstructFromUnexpected_RvalueCompilesAndWorks)
	{
		// Arrange.
		auto expected = Expected<std::string, std::string>(Unexpected<std::string>(std::string("FAIL")));

		// Act.
		const auto& error = expected.GetError();

		// Assert.
		ASSERT_EQ(error, "FAIL");
	}

	TEST(Unexpected, Create_StoresError)
	{
		// Arrange & Act.
		auto unexpected = Unexpected<std::string>::Create(std::string("FAIL"));

		// Assert.
		ASSERT_EQ(unexpected.Error, "FAIL");
	}
}
