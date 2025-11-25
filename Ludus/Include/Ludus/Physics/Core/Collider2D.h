#pragma once

#include <cstdint>

#include <Ludus/Engine/Entity.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Math/AABB.h>
#include <Ludus/Math/Circle.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/Core/BodyType.h>

namespace Ludus::Physics::Core
{
	using ColliderHandle = uint32_t;
	using Index = uint8_t;

	struct Collider2D
	{
	private:
		inline static ColliderHandle s_NextHandle = 1;

	public:
		ColliderHandle Handle;
		Ludus::Engine::EntityHandle OwnerHandle;
		Index LayerIndex;
		Ludus::Engine::LayerMask CollidesWith;
		bool IsTrigger;

		Collider2D(
			Ludus::Engine::EntityHandle owner,
			Index layerIndex,
			Ludus::Engine::LayerMask collidesWith = Ludus::Engine::LayerMask::GetEmpty(),
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

		Ludus::Math::AABB ToAABB(const Ludus::Math::Transform2D& transform) const
		{
			return Ludus::Math::AABB::FromTransform(transform);
		}

		Ludus::Math::Circle ToCircle(const Ludus::Math::Transform2D& transform) const
		{
			return Ludus::Math::Circle::FromTransform(transform);
		}
	};
}
