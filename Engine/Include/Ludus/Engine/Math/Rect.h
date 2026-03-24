#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Math
{
	struct Rect
	{
		Vector2D Position;
		Vector2D Size;

		static Rect Create(Vector2D position, Vector2D size)
		{
			return { position, size };
		}
	};
}
