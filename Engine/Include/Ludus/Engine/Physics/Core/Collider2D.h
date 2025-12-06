#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/AABB.h>
#include <Ludus/Engine/Math/Circle.h>
#include <Ludus/Engine/Math/Transform2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Engine::Physics::Core
{
	using ColliderHandle = uint32_t;
	using Index = uint8_t;

	struct Collider2D
	{
	private:
		inline static ColliderHandle s_NextHandle = 1;

	public:
		ColliderHandle Handle;
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		Index LayerIndex;
		Ludus::Engine::Physics::Core::LayerMask CollidesWith;
		bool IsTrigger;

		Collider2D(
			Ludus::Engine::Core::EntityHandle owner,
			Index layerIndex,
			Ludus::Engine::Physics::Core::LayerMask collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty(),
			bool isTrigger = false
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			LayerIndex(layerIndex),
			CollidesWith(collidesWith),
			IsTrigger(isTrigger)
		{ }

		Collider2D(const Collider2D&) = delete;
		Collider2D& operator=(const Collider2D&) = delete;
		Collider2D(Collider2D&&) noexcept = default;
		Collider2D& operator=(Collider2D&&) noexcept = default;
		~Collider2D() = default;

		bool operator==(const Collider2D& other) const { return Handle == other.Handle; }

		Ludus::Engine::Math::AABB ToAABB(const Ludus::Engine::Math::Transform2D& transform) const
		{
			return Ludus::Engine::Math::AABB::FromTransform(transform);
		}

		Ludus::Engine::Math::Circle ToCircle(const Ludus::Engine::Math::Transform2D& transform) const
		{
			return Ludus::Engine::Math::Circle::FromTransform(transform);
		}
	};
}
