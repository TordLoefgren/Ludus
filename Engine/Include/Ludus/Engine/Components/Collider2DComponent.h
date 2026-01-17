#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/AABB.h>
#include <Ludus/Engine/Math/Circle.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Engine::Components
{
	struct Collider2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		Ludus::Engine::Physics::Core::LayerIndex LayerIndex;
		Ludus::Engine::Physics::Core::LayerMask CollidesWith;
		bool IsTrigger;

		explicit Collider2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			Ludus::Engine::Physics::Core::LayerIndex layerIndex = 0,
			Ludus::Engine::Physics::Core::LayerMask collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty(),
			bool isTrigger = false
		) :
			OwnerHandle(owner),
			LayerIndex(layerIndex),
			CollidesWith(collidesWith),
			IsTrigger(isTrigger)
		{ }

		~Collider2DComponent() = default;

		bool operator==(const Collider2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }

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
