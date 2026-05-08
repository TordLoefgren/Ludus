#pragma once

#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Math
{
	struct Rect
	{
		Vector2D Position { };
		Size<float> Size { };

		static Rect Create(Vector2D position, Vector2D size)
		{
			return { position, { size.X, size.Y } };
		}

		static Rect Empty()
		{
			return { };
		}

		bool IsEmpty() const
		{
			return Position.X == 0.0f && Position.Y == 0.0f && Size.Width == 0.0f && Size.Height == 0.0f;
		}
	};
}
