#include <Ludus/Graphics/Font.h>

namespace Ludus::Graphics
{
	Font::Font()
		: m_GlyphMap(), m_FT_Library(nullptr), m_Face(nullptr), m_PixelSize(48)
	{
		if (FT_Init_FreeType(&m_FT_Library))
		{
			LUDUS_LOG_ERROR("Could not initialize FreeType Library");
		}

		if (FT_New_Face(m_FT_Library, "Resources/Fonts/ARIAL.TTF", 0, &m_Face))
		{
			LUDUS_LOG_ERROR("Failed to load font");
		}

		FT_Set_Pixel_Sizes(m_Face, 0, m_PixelSize);
	}

	Font::~Font()
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

	const Glyph* Font::GetGlyph(const char character)
	{
		if (auto it = m_GlyphMap.find(character); it != m_GlyphMap.end())
		{
			return &it->second;
		}

		if (FT_Load_Char(m_Face, character, FT_LOAD_RENDER))
		{
			LUDUS_LOG_ERROR("Failed to load Glyph");
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

	float Font::MeasureTextWidth(std::string_view text)
	{
		auto widthPixels = 0.0f;
		for (auto c : text)
		{
			const auto* glyph = GetGlyph(c);
			if (glyph)
			{
				widthPixels += glyph->Advance / 64.0f;
			}
		}

		return widthPixels;
	}
}
