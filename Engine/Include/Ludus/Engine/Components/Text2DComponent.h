#pragma once

#include <string>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>

namespace Ludus::Engine::Components
{
	struct Text2DComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle {};
		std::string Text = "";
		Ludus::Engine::Graphics::Color Color = Ludus::Engine::Graphics::Colors::White;
		Ludus::Engine::Graphics::HorizontalTextAlignment HorizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left;

		Text2DComponent() = default;

		explicit Text2DComponent(
			std::string text,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		) :
			Text(text),
			Color(color),
			HorizontalAlignment(horizontalAlignment)
		{ }

		Text2DComponent(
			Ludus::Engine::Core::EntityHandle owner,
			std::string text = "",
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		) :
			OwnerHandle(owner),
			Text(text),
			Color(color),
			HorizontalAlignment(horizontalAlignment)
		{ }

		~Text2DComponent() = default;

		bool operator==(const Text2DComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}

