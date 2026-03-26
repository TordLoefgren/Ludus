#pragma once

#include <filesystem>
#include <ft2build.h>
#include <string_view>
#include <unordered_map>
#include FT_FREETYPE_H

#include <Ludus/Engine/Graphics/Glyph.h>

namespace Ludus::Engine::Graphics
{
	class Font
	{
	private:
		FT_Library m_FT_Library;
		FT_Face m_Face;
		int m_PixelSize;
		std::unordered_map<char, Glyph> m_GlyphMap;
	public:
		explicit Font(std::filesystem::path fontPath);
		~Font();

		const Glyph* GetGlyph(const char character);
		float MeasureTextWidth(std::string_view text);
	};
}
