#pragma once

#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	struct Glyph
	{
		Texture Texture;
		int Width;
		int Rows;
		int BearingX;
		int BearingY;
		int Advance;
	};
}
