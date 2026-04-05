#pragma once

#include <string>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>

namespace Ludus::Engine::Components
{
	struct Text2DComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
		std::string Text = "";
		Ludus::Engine::Graphics::Color Color = Ludus::Engine::Graphics::Colors::White;
		Ludus::Engine::Graphics::HorizontalTextAlignment HorizontalTextAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left;

		Text2DComponent() = default;

		explicit Text2DComponent(
			std::string text,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalTextAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		) :
			Text(text),
			Color(color),
			HorizontalTextAlignment(horizontalTextAlignment)
		{ }

		Text2DComponent(
			Ludus::Engine::Core::EntityId owner,
			std::string text = "",
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalTextAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		) :
			OwnerId(owner),
			Text(text),
			Color(color),
			HorizontalTextAlignment(horizontalTextAlignment)
		{ }

		~Text2DComponent() = default;

		bool operator==(const Text2DComponent& other) const { return OwnerId == other.OwnerId; }
	};
}

