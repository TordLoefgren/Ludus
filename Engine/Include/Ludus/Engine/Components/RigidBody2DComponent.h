#pragma once

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Engine::Components
{
	struct RigidBody2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle { };
		Ludus::Engine::Math::Vector2D Velocity;
		float GravityScale = 1.0f;
		float Mass = 1.0f;
		Ludus::Engine::Physics::Core::BodyType BodyType = Ludus::Engine::Physics::Core::BodyType::Dynamic;

		RigidBody2DComponent() = default;

		explicit RigidBody2DComponent(
			Ludus::Engine::Math::Vector2D velocity,
			Ludus::Engine::Physics::Core::BodyType bodyType = Ludus::Engine::Physics::Core::BodyType::Dynamic,
			float gravityScale = 1.0f,
			float mass = 1.0f
		) :
			Velocity(velocity),
			BodyType(bodyType),
			GravityScale(gravityScale),
			Mass(mass)
		{ }

		RigidBody2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			Ludus::Engine::Math::Vector2D velocity = { 0.0f, 0.0f },
			Ludus::Engine::Physics::Core::BodyType bodyType = Ludus::Engine::Physics::Core::BodyType::Dynamic,
			float gravityScale = 1.0f,
			float mass = 1.0f
		) :
			OwnerHandle(owner),
			Velocity(velocity),
			BodyType(bodyType),
			GravityScale(gravityScale),
			Mass(mass)
		{ }

		~RigidBody2DComponent() = default;

		bool operator==(const RigidBody2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}
