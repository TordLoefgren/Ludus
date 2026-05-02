#include "pch.h"

#include <utility>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/Font.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	Font::Font(std::filesystem::path fontPath)
		: m_GlyphMap(), m_FT_Library(nullptr), m_Face(nullptr), m_PixelSize(48)
	{
		if (FT_Init_FreeType(&m_FT_Library))
		{
			LUDUS_LOG_ERROR("Could not initialize FreeType Library");
		}

		if (FT_New_Face(m_FT_Library, fontPath.string().c_str(), 0, &m_Face))
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
		if (auto iter = m_GlyphMap.find(character); iter != m_GlyphMap.end())
		{
			return &iter->second;
		}

		if (FT_Load_Char(m_Face, character, FT_LOAD_RENDER))
		{
			LUDUS_LOG_ERROR("Failed to load Glyph");
			return nullptr;
		}

		auto slot = m_Face->glyph;
		auto bitmap = m_Face->glyph->bitmap;

		auto texture = Texture::FromR8(bitmap.width, bitmap.rows, bitmap.buffer);

		Glyph glyph
		{
			std::move(texture),
			static_cast<int>(bitmap.width),
			static_cast<int>(bitmap.rows),
			static_cast<int>(slot->bitmap_left),
			static_cast<int>(slot->bitmap_top),
			static_cast<int>(slot->advance.x),
		};

		auto [iter, _] = m_GlyphMap.emplace(character, std::move(glyph));
		return &iter->second;
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
