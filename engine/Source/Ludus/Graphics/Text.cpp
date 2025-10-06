#include <Ludus/Graphics/Text.h>

namespace Ludus::Graphics
{
	Text::Text()
		: m_GlyphMap(), m_FT_Library(nullptr), m_Face(nullptr), m_PixelSize(48)
	{
		if (FT_Init_FreeType(&m_FT_Library))
		{
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		}

		if (FT_New_Face(m_FT_Library, "Resources/Fonts/ARIAL.TTF", 0, &m_Face))
		{
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}

		FT_Set_Pixel_Sizes(m_Face, 0, m_PixelSize);
	}

	Text::~Text()
	{
		if (m_Face)
		{
			FT_Done_Face(m_Face);
		}

		if (m_FT_Library)
		{
			FT_Done_FreeType(m_FT_Library);
		}
	}

	const Glyph* Text::GetGlyph(const char character)
	{
		if (auto it = m_GlyphMap.find(character); it != m_GlyphMap.end())
		{
			return &it->second;
		}

		if (FT_Load_Char(m_Face, character, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			return nullptr;
		}

		auto slot = m_Face->glyph;
		auto bitmap = m_Face->glyph->bitmap;

		auto texture = Texture::FromMemory(bitmap.width, bitmap.rows, bitmap.buffer);

		Glyph glyph
		{
			std::move(texture),
			(int)bitmap.width,
			(int)bitmap.rows,
			(int)slot->bitmap_left,
			(int)slot->bitmap_top,
			(int)slot->advance.x,
		};

		auto [it, _] = m_GlyphMap.emplace(character, std::move(glyph));
		return &it->second;
	}
}
