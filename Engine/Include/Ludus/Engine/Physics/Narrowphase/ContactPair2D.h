#pragma once

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Narrowphase/ContactPoint2D.h>

namespace Ludus::Engine::Physics::Narrowphase
{
	struct ContactPair2D
	{
		Ludus::Engine::Core::EntityId EntityIdA;
		Ludus::Engine::Core::EntityId EntityIdB;
		size_t WorldIndexA;
		size_t WorldIndexB;
		ContactPoint2D Point;
		bool IsTriggerPair = false;

		void AddContactPoint(
			const Ludus::Engine::Math::Vector2D& LocalA,
			const Ludus::Engine::Math::Vector2D& LocalB,
			const Ludus::Engine::Math::Vector2D& normal,
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
