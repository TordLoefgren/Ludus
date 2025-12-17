#pragma once

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Engine::Components
{
	struct RigidBody2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		Ludus::Engine::Math::Vector2D Velocity;
		float GravityScale;
		float Mass;
		float InverseMass;
		Ludus::Engine::Physics::Core::BodyType Type;

		RigidBody2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			Ludus::Engine::Math::Vector2D velocity,
			Ludus::Engine::Physics::Core::BodyType type = Ludus::Engine::Physics::Core::BodyType::Dynamic,
			float gravityScale = 1.0f,
			float mass = 1.0f
		) :
			OwnerHandle(owner),
			Velocity(velocity),
			Type(type),
			GravityScale(gravityScale),
			Mass(mass),
			InverseMass(1 / mass)
		{ }

		~RigidBody2DComponent() = default;

		bool operator==(const RigidBody2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}
