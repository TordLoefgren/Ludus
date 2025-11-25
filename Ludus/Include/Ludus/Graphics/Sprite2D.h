#pragma once

#include <cstdint>

#include <Ludus/Engine/Entity.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/Shape.h>
#include <Ludus/Graphics/Texture.h>

namespace Ludus::Graphics
{
	using SpriteHandle = uint32_t;

	struct Sprite2D
	{
	private:
		inline static SpriteHandle s_NextHandle = 1;

	public:
		SpriteHandle Handle;
		Ludus::Engine::EntityHandle OwnerHandle;
		Ludus::Graphics::Shape Shape;
		Ludus::Graphics::Color Color;
		Ludus::Graphics::Texture* Texture = nullptr;
		bool Fill = true;

		Sprite2D(
			Ludus::Engine::EntityHandle owner,
			Ludus::Graphics::Shape shape,
			Ludus::Graphics::Color color = Ludus::Graphics::Colors::White,
			Ludus::Graphics::Texture* texture = nullptr,
			bool fill = true
		) :
			Handle(s_NextHandle++),
			OwnerHandle(owner),
			Shape(shape),
			Color(color),
			Texture(texture),
			Fill(fill)
		{ }

		Sprite2D(const Sprite2D&) = delete;
		Sprite2D& operator=(const Sprite2D&) = delete;
		Sprite2D(Sprite2D&&) noexcept = default;
		Sprite2D& operator=(Sprite2D&&) noexcept = default;
		~Sprite2D() = default;

		bool operator==(const Sprite2D& other) const { return Handle == other.Handle; }
	};
}

