#pragma once

#include <cstdint>

#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Math/Bounds2D.h>
#include <Ludus/Math/Transform2D.h>

namespace Ludus::Physics
{
	using Ludus::Engine::GameObjectHandle;
	using Ludus::Engine::LayerMask;
	using Ludus::Math::Bounds2D;
	using Ludus::Math::Transform2D;

	using ColliderHandle = uint32_t;
	using Index = uint8_t;

	struct Collider2D
	{
	private:
		inline static ColliderHandle s_NextHandle = 1;

	public:
		ColliderHandle Handle;
		GameObjectHandle OwnerHandle;
		Index LayerIndex;
		LayerMask CollidesWith;
		bool IsStatic;

		Collider2D(
			GameObjectHandle owner,
			Index layerIndex,
			LayerMask collidesWith = LayerMask::GetEmpty(),
			bool isStatic = false
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			LayerIndex(layerIndex),
			CollidesWith(collidesWith),
			IsStatic(isStatic)
		{ }

		Collider2D(const Collider2D&) = delete;
		Collider2D& operator=(const Collider2D&) = delete;
		Collider2D(Collider2D&&) noexcept = default;
		Collider2D& operator=(Collider2D&&) noexcept = default;
		~Collider2D() = default;

		bool operator==(const Collider2D& other) const { return Handle == other.Handle; }

		Bounds2D ToAABB(const Transform2D& transform) const
		{
			return Bounds2D::Create(transform.Position, transform.Scale);
		}
	};
}
