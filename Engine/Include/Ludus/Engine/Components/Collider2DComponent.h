#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Math/AABB.h>
#include <Ludus/Engine/Math/Circle.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Engine::Components
{
	struct Collider2DComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
		Ludus::Engine::Physics::Core::LayerIndex LayerIndex = 0;
		Ludus::Engine::Physics::Core::LayerMask CollidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty();
		bool IsTrigger = false;

		Collider2DComponent() = default;

		explicit Collider2DComponent(
			Ludus::Engine::Physics::Core::LayerIndex layerIndex,
			Ludus::Engine::Physics::Core::LayerMask collidesWith,
			bool isTrigger = false
		) :
			LayerIndex(layerIndex),
			CollidesWith(collidesWith),
			IsTrigger(isTrigger)
		{ }

		Collider2DComponent(
			Ludus::Engine::Core::EntityId owner,
			Ludus::Engine::Physics::Core::LayerIndex layerIndex = 0,
			Ludus::Engine::Physics::Core::LayerMask collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty(),
			bool isTrigger = false
		) :
			OwnerId(owner),
			LayerIndex(layerIndex),
			CollidesWith(collidesWith),
			IsTrigger(isTrigger)
		{ }

		~Collider2DComponent() = default;

		bool operator==(const Collider2DComponent& other) const { return OwnerId == other.OwnerId; }

		Ludus::Engine::Math::AABB ToAABB(const Ludus::Engine::Components::Transform2DComponent& transform) const
		{
			return Ludus::Engine::Math::AABB::FromTransform(transform);
		}

		Ludus::Engine::Math::Circle ToCircle(const Ludus::Engine::Components::Transform2DComponent& transform) const
		{
			return Ludus::Engine::Math::Circle::FromTransform(transform);
		}
	};
}
