#pragma once

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Components
{
	struct Camera2DComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
		float OrthographicSize = 10.0f;
		int Priority = -1;

		Camera2DComponent() = default;

		explicit Camera2DComponent(float orthographicSize, int priority = -1)
			: OrthographicSize(orthographicSize), Priority(priority)
		{ }

		Camera2DComponent(Ludus::Engine::Core::EntityId owner, float orthographicSize = 10.0f, int priority = -1)
			: OwnerId(owner), OrthographicSize(orthographicSize), Priority(priority)
		{ }

		~Camera2DComponent() = default;

		bool operator==(const Camera2DComponent& other) const { return OwnerId == other.OwnerId; }
	};
}
