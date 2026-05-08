#pragma once

namespace Ludus::Engine::Math
{
	struct RectInt
	{
		int X = 0;
		int Y = 0;
		int Width = 0;
		int Height = 0;

		static RectInt Create(int x, int y, int width, int height)
		{
			return { x, y, width, height };
		}

		static RectInt Empty()
		{
			return { };
		}

		bool IsEmpty() const
		{
			return X == 0 && Y == 0 && Width == 0 && Height == 0;
		}
	};
}
