#pragma once

#include <cmath>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Components
{
	struct Transform2DComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
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
			Ludus::Engine::Core::EntityId ownerId,
			Ludus::Engine::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Engine::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		) :
			OwnerId(ownerId),
			Position(position),
			Scale(scale),
			Rotation(rotation)
		{ }

		~Transform2DComponent() = default;

		bool operator==(const Transform2DComponent& other) const { return OwnerId == other.OwnerId; }
	};
}
