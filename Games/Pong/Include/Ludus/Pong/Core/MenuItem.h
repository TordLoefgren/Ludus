#pragma once

#include <string>

#include <Ludus/Engine/Entity.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Graphics/Shape.h>
#include <Ludus/Math/Transform2D.h>

namespace Ludus::Pong::Core
{
	struct MenuItem
	{
		Ludus::Engine::EntityHandle Handle;
		std::string MenuText;
		Ludus::Math::Transform2D Transform;
		Ludus::Graphics::Color Color = Ludus::Graphics::Colors::LightGray;
		Ludus::Graphics::HorizontalTextAlignment HorizontalTextAlignment = Ludus::Graphics::HorizontalTextAlignment::Center;
	};
}
