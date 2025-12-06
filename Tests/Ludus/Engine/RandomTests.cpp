#include "pch.h"

#include <stdexcept>

namespace Ludus::Tests::Engine
{
	using Ludus::Engine::Core::Random;

	TEST(Random, NextInt_ReturnsValueBetweenMinMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 1;
		auto v2 = 2;

		// Act.
		auto value = random.NextInt(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextInt_ReturnsValueBetweenMinMax_When_MinIsNegative)
	{
		// Arrange.
		auto random = Random();
		auto v1 = -1;
		auto v2 = 2;

		// Act.
		auto value = random.NextInt(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextInt_ThrowsInvalidArgumentException_When_MinGreaterThanMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 2;
		auto v2 = 1;

		// Act & Assert.
		EXPECT_THROW(random.NextInt(v1, v2), std::invalid_argument);
	}

	TEST(Random, NextUint_ReturnsValueBetweenMinMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 1u;
		auto v2 = 2u;

		// Act.
		auto value = random.NextUint(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextUint_ThrowsInvalidArgumentException_When_MinGreaterThanMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 2u;
		auto v2 = 1u;

		// Act & Assert.
		EXPECT_THROW(random.NextUint(v1, v2), std::invalid_argument);
	}

	TEST(Random, NextFloat_ReturnsValueBetweenMinMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 1.0f;
		auto v2 = 2.0f;

		// Act.
		auto value = random.NextFloat(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextFloat_ReturnsValueBetweenMinMax_When_MinIsNegative)
	{
		// Arrange.
		auto random = Random();
		auto v1 = -1.0f;
		auto v2 = 2.0f;

		// Act.
		auto value = random.NextFloat(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextFloat_ThrowsInvalidArgumentException_When_MinGreaterThanMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 2.0f;
		auto v2 = 1.0f;

		// Act & Assert.
		EXPECT_THROW(random.NextFloat(v1, v2), std::invalid_argument);
	}

	TEST(Random, NextDouble_ReturnsValueBetweenMinMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 1.0;
		auto v2 = 2.0;

		// Act.
		auto value = random.NextDouble(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextDouble_ReturnsValueBetweenMinMax_When_MinIsNegative)
	{
		// Arrange.
		auto random = Random();
		auto v1 = -1.0;
		auto v2 = 2.0;

		// Act.
		auto value = random.NextDouble(v1, v2);

		// Assert.
		EXPECT_GE(value, v1);
		EXPECT_LE(value, v2);
	}

	TEST(Random, NextDouble_ThrowsInvalidArgumentException_When_MinGreaterThanMax)
	{
		// Arrange.
		auto random = Random();
		auto v1 = 2.0;
		auto v2 = 1.0;

		// Act & Assert.
		EXPECT_THROW(random.NextDouble(v1, v2), std::invalid_argument);
	}
}
