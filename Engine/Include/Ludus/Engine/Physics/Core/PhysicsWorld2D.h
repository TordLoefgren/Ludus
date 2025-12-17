#pragma once

#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>

namespace Ludus::Engine::Physics::Core
{
	struct PhysicsWorld2D
	{
		std::vector<Ludus::Engine::Core::EntityHandle> Entities;
		std::vector<Ludus::Engine::Components::Collider2DComponent*> Colliders;
		std::vector<Ludus::Engine::Components::RigidBody2DComponent*> RigidBodies;
		std::vector<Ludus::Engine::Components::Transform2DComponent*> Transforms;

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
