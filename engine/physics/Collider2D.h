#pragma once

#include "..\graphics\Transform2D.h"
#include "Bounds2D.h"
#include "LayerMask.h"
#include <cstdint>

enum class Shape : uint8_t { Box, Circle };

using ColliderHandle = uint32_t;
using GameObjectHandle = uint32_t;
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
	// TODO: Implement shape.

	Collider2D(GameObjectHandle owner, Index layer, LayerMask collidesWith = LayerMask::GetEmpty(), bool isStatic = false)
		: Handle(s_NextHandle++), OwnerHandle(owner), LayerIndex(layer), CollidesWith(collidesWith), IsStatic(isStatic)
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
