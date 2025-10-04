#pragma once

#include "Texture.h"
#include <ft2build.h>
#include <iostream>
#include <unordered_map>
#include FT_FREETYPE_H

struct Glyph
{
	Texture Texture;
	int Width;
	int Rows;
	int BearingX;
	int BearingY;
	int Advance;
};

class Text
{
private:
	FT_Library m_FT_Library;
	FT_Face m_Face;
	int m_PixelSize;
	std::unordered_map<char, Glyph> m_GlyphMap;
public:
	Text();
	~Text();

	const Glyph* GetGlyph(const char character);
};

