#pragma once

#include <cstdint>

#include <Ludus/Engine/Entity.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Core/BodyType.h>

namespace Ludus::Physics::Core
{
	using RigidBodyHandle = uint32_t;

	struct RigidBody2D
	{
	private:
		inline static RigidBodyHandle s_NextHandle = 1;

	public:
		RigidBodyHandle Handle;
		Ludus::Engine::EntityHandle OwnerHandle;
		Ludus::Math::Vector2D Velocity;
		float GravityScale;
		float Mass;
		float InverseMass;
		BodyType Type;

		RigidBody2D(
			Ludus::Engine::EntityHandle owner,
			Ludus::Math::Vector2D velocity,
			BodyType type = BodyType::Dynamic,
			float gravityScale = 1.0f,
			float mass = 1.0f
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			Velocity(velocity),
			Type(type),
			GravityScale(gravityScale),
			Mass(mass),
			InverseMass(1 / mass)
		{ }

		RigidBody2D(const RigidBody2D&) = delete;
		RigidBody2D& operator=(const RigidBody2D&) = delete;
		RigidBody2D(RigidBody2D&&) noexcept = default;
		RigidBody2D& operator=(RigidBody2D&&) noexcept = default;
		~RigidBody2D() = default;

		bool operator==(const RigidBody2D& other) const { return Handle == other.Handle; }
	};
}
