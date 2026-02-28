#pragma once

#include <cmath>

#include <Ludus/Engine/Core/Entity.h>  
#include <Ludus/Engine/Math/Numeric.h>  
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Components
{
	struct Transform2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle {};
		Ludus::Engine::Math::Vector2D Position { 0.0f, 0.0f };
		Ludus::Engine::Math::Vector2D Scale { 1.0f, 1.0f };
		float Rotation { 0.0f };

		Transform2DComponent() = default;

		explicit Transform2DComponent(
			Ludus::Engine::Math::Vector2D position,
			Ludus::Engine::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		) :
			Position(position),
			Scale(scale),
			Rotation(rotation)
		{ }

		Transform2DComponent(
			Ludus::Engine::Core::EntityHandle ownerHandle,
			Ludus::Engine::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Engine::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		) :
			OwnerHandle(ownerHandle),
			Position(position),
			Scale(scale),
			Rotation(rotation)
		{ }

		~Transform2DComponent() = default;

		bool operator==(const Transform2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }

		/// <summary>
		/// Computes the forward unit vector from the transform's rotation.
		/// </summary>
		/// <returns>A unit vector pointing in the forward direction.</returns>
		Ludus::Engine::Math::Vector2D Forward() const
		{
			const float r = Ludus::Engine::Math::Numeric::DegreesToRadians(Rotation);
			return { std::cos(r), std::sin(r) };
		}

		void Rotate(const Ludus::Engine::Math::Vector2D& unitVector)
		{
			Rotation = Ludus::Engine::Math::Numeric::RotationDegreesFromDirection(unitVector.X, unitVector.Y);
		}
	};
}
