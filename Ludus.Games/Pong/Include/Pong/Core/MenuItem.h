#pragma once

#include <string>

#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Math/Transform2D.h>

namespace Pong::Core
{
	struct MenuItem
	{
		std::string MenuText;
		Ludus::Math::Transform2D Transform;
		Ludus::Graphics::Color ActiveColor = Ludus::Graphics::Colors::White;
		Ludus::Graphics::Color InactiveColor = Ludus::Graphics::Colors::LightGray;
		Ludus::Graphics::HorizontalTextAlignment HorizontalTextAlignment = Ludus::Graphics::HorizontalTextAlignment::Center;
	};
}
