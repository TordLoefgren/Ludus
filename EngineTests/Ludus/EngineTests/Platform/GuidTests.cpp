#include "pch.h"

#include <string_view>

#include <Ludus/Engine/Platform/Guid.h>

namespace Ludus::EngineTests::Platform
{
	using Guid = Ludus::Engine::Platform::Guid;

	TEST(Guid, DefaultConstructedGuid_IsEmpty)
	{
		// Arrange.
		const Guid guid { };

		// Act & Assert.
		ASSERT_TRUE(guid.IsEmpty());
		EXPECT_EQ(guid.ToString(), "00000000-0000-0000-0000-000000000000");
	}

	TEST(Guid, TryParse_ReturnsGuid_When_TextIsValidLowercaseGuid)
	{
		// Arrange.
		const auto text = std::string_view("00112233-4455-6677-8899-aabbccddeeff");

		// Act.
		const auto guid = Ludus::Engine::Platform::TryParse(text);

		// Assert.
		ASSERT_TRUE(guid.has_value());
		EXPECT_EQ(guid->ToString(), text);
		EXPECT_FALSE(guid->IsEmpty());
	}

	TEST(Guid, TryParse_ReturnsGuid_When_TextIsValidUppercaseGuid)
	{
		// Arrange.
		const auto text = std::string_view("00112233-4455-6677-8899-AABBCCDDEEFF");

		// Act.
		const auto guid = Ludus::Engine::Platform::TryParse(text);

		// Assert.
		ASSERT_TRUE(guid.has_value());
		EXPECT_EQ(guid->ToString(), "00112233-4455-6677-8899-aabbccddeeff");
	}

	TEST(Guid, TryParse_ReturnsNullopt_When_TextIsInvalid)
	{
		// Arrange.
		const auto text = std::string_view("not-a-guid");

		// Act.
		const auto guid = Ludus::Engine::Platform::TryParse(text);

		// Assert.
		EXPECT_FALSE(guid.has_value());
	}

	TEST(Guid, CreateGuid_ReturnsNonEmptyGuidThatRoundTripsThroughString)
	{
		// Arrange & Act.
		const auto guid = Ludus::Engine::Platform::CreateGuid();
		const auto guidText = guid.ToString();
		const auto reparsedGuid = Ludus::Engine::Platform::TryParse(guidText);

		// Assert.
		EXPECT_FALSE(guid.IsEmpty());
		ASSERT_TRUE(reparsedGuid.has_value());
		EXPECT_EQ(*reparsedGuid, guid);
	}
}
