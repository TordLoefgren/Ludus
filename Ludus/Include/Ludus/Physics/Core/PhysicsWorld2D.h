#pragma once

#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/Core/Collider2D.h>

namespace Ludus::Physics::Core
{
	struct PhysicsWorld2D
	{
		std::vector<Ludus::Engine::EntityHandle> Entities;
		std::vector<Ludus::Physics::Core::Collider2D*> Colliders;
		std::vector<Ludus::Math::Transform2D*> Transforms;

		void Clear()
		{
			Entities.clear();
			Colliders.clear();
			Transforms.clear();
		}

		size_t Size() const { return Colliders.size(); }
	};
}
