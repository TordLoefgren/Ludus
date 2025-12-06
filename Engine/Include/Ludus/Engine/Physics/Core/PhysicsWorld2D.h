#pragma once

#include <Ludus/Engine/Math/Transform2D.h>
#include <Ludus/Engine/Physics/Core/Collider2D.h>

namespace Ludus::Engine::Physics::Core
{
	struct PhysicsWorld2D
	{
		std::vector<Ludus::Engine::Core::EntityHandle> Entities;
		std::vector<Ludus::Engine::Physics::Core::Collider2D*> Colliders;
		std::vector<Ludus::Engine::Physics::Core::RigidBody2D*> RigidBodies;
		std::vector<Ludus::Engine::Math::Transform2D*> Transforms;

		void Clear()
		{
			Entities.clear();
			Colliders.clear();
			RigidBodies.clear();
			Transforms.clear();
		}

		size_t Size() const { return Colliders.size(); }
	};
}
