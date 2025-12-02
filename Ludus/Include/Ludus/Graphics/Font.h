#pragma once

#include <ft2build.h>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include FT_FREETYPE_H

#include <Ludus/Graphics/Texture.h>
#include <Ludus/Debug/Debug.h>
#include <Ludus/Graphics/Glyph.h>

namespace Ludus::Graphics
{
	class Font
	{
	private:
		FT_Library m_FT_Library;
		FT_Face m_Face;
		int m_PixelSize;
		std::unordered_map<char, Glyph> m_GlyphMap;
	public:
		Font();
		~Font();

		const Glyph* GetGlyph(const char character);
		float MeasureTextWidth(std::string_view text);
	};
}
