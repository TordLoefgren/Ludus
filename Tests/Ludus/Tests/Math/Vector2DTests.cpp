#include "pch.h"

namespace Ludus::Tests::Math
{
	using Ludus::Engine::Math::Vector2D;

	namespace Constants = Ludus::Engine::Math::Constants;

	TEST(Vector2D, Dot_ReturnsZero_When_VectorsAreOrthogonal)
	{
		// Arrange.
		auto v1 = Vector2D(1.0f, 0.0f);
		auto v2 = Vector2D(0.0f, 5.0f);

		// Act.
		auto dot = Vector2D::Dot(v1, v2);

		// Assert.
		EXPECT_NEAR(dot, 0.0f, Constants::EPSILON);
	}

	TEST(Vector2D, Dot_ReturnsPositive_When_AngleIsAcute)
	{
		// Arrange.
		auto v1 = Vector2D(1.0f, 2.0f);
		auto v2 = Vector2D(2.0f, 1.0f);

		// Act.
		auto dot = Vector2D::Dot(v1, v2);

		// Assert.
		EXPECT_NEAR(dot, 4.0f, Constants::EPSILON);
	}

	TEST(Vector2D, Dot_ReturnsNegative_When_AngleIsObtuse)
	{
		// Arrange.
		auto v1 = Vector2D(1.0f, 0.0f);
		auto v2 = Vector2D(-1.0f, 1.0f);

		// Act.
		auto dot = Vector2D::Dot(v1, v2);

		// Assert.
		EXPECT_NEAR(dot, -1.0f, Constants::EPSILON);
	}
}
