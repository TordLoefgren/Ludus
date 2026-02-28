#pragma once

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Math/Numeric.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Components
{
	struct Camera2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle {};
		float OrthographicSize = 10.0f;
		int Priority = -1;

		Camera2DComponent() = default;

		explicit Camera2DComponent(float orthographicSize, int priority = -1)
			: OrthographicSize(orthographicSize), Priority(priority)
		{ }

		Camera2DComponent(Ludus::Engine::Core::EntityHandle owner, float orthographicSize = 10.0f, int priority = -1)
			: OwnerHandle(owner), OrthographicSize(orthographicSize), Priority(priority)
		{ }

		~Camera2DComponent() = default;

		bool operator==(const Camera2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}
