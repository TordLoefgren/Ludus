#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	using SpriteHandle = uint32_t;

	struct Sprite2D
	{
	private:
		inline static SpriteHandle s_NextHandle = 1;

	public:
		SpriteHandle Handle;
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		Ludus::Engine::Graphics::Shape Shape;
		Ludus::Engine::Graphics::Color Color;
		Ludus::Engine::Graphics::Texture* Texture = nullptr;
		bool Fill = true;

		Sprite2D(
			Ludus::Engine::Core::EntityHandle owner,
			Ludus::Engine::Graphics::Shape shape,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::Texture* texture = nullptr,
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

