#pragma once

#include <cstdint>
#include <string>

#include <Ludus/Engine/Entity.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>

namespace Ludus::Graphics
{
	using TextHandle = uint32_t;

	struct Text2D
	{
	private:
		inline static TextHandle s_NextHandle = 1;

	public:
		TextHandle Handle;
		Ludus::Engine::EntityHandle OwnerHandle;
		std::string Text;
		Ludus::Graphics::Color Color { Ludus::Graphics::Colors::White };
		HorizontalTextAlignment HorizontalAlignment { HorizontalTextAlignment::Left };

		Text2D(
			Ludus::Engine::EntityHandle owner,
			std::string text,
			Ludus::Graphics::Color color,
			HorizontalTextAlignment horizontalAlignment
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			Text(text),
			Color(color),
			HorizontalAlignment(horizontalAlignment)
		{ }

		Text2D(const Text2D&) = delete;
		Text2D& operator=(const Text2D&) = delete;
		Text2D(Text2D&&) noexcept = default;
		Text2D& operator=(Text2D&&) noexcept = default;
		~Text2D() = default;

		bool operator==(const Text2D& other) const { return Handle == other.Handle; }
	};
}

