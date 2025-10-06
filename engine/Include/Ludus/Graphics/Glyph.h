#pragma once

#include <Ludus/Graphics/Texture.h>

namespace Ludus::Graphics
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
