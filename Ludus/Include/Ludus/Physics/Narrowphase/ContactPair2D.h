#pragma once

#include <Ludus/Engine/Entity.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Narrowphase/ContactPoint2D.h>

namespace Ludus::Physics::Narrowphase
{
	struct ContactPair2D
	{
		Ludus::Engine::EntityHandle EntityHandleA;
		Ludus::Engine::EntityHandle EntityHandleB;
		size_t WorldIndexA;
		size_t WorldIndexB;
		ContactPoint2D Point;
		bool IsTriggerPair = false;

		void AddContactPoint(
			const Ludus::Math::Vector2D& LocalA,
			const Ludus::Math::Vector2D& LocalB,
			const Ludus::Math::Vector2D& normal,
			float point
		)
		{
			Point.LocalA = LocalA;
			Point.LocalB = LocalB;
			Point.Normal = normal;
			Point.Penetration = point;
		}
	};
}
