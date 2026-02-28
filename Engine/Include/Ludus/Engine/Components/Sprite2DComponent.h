#pragma once

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Components
{
	struct Sprite2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle {};
		Ludus::Engine::Graphics::Shape Shape = Ludus::Engine::Graphics::Shape::Quad;
		Ludus::Engine::Graphics::Color Color = Ludus::Engine::Graphics::Colors::White;
		Ludus::Engine::Graphics::Texture* Texture = nullptr;
		bool Fill = true;

		Sprite2DComponent() = default;

		explicit Sprite2DComponent(
			Ludus::Engine::Graphics::Shape shape,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::Texture* texture = nullptr,
			bool fill = true
		) :
			Shape(shape),
			Color(color),
			Texture(texture),
			Fill(fill)
		{ }

		Sprite2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			Ludus::Engine::Graphics::Shape shape = Ludus::Engine::Graphics::Shape::Quad,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::Texture* texture = nullptr,
			bool fill = true
		) :
			OwnerHandle(owner),
			Shape(shape),
			Color(color),
			Texture(texture),
			Fill(fill)
		{ }

		~Sprite2DComponent() = default;

		bool operator==(const Sprite2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}

