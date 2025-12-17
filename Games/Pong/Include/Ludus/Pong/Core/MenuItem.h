#pragma once

#include <string>

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>

namespace Ludus::Pong::Core
{
	struct MenuItem
	{
		Ludus::Engine::Core::EntityHandle Handle;
		std::string MenuText;
		Ludus::Engine::Components::Transform2DComponent Transform;
		Ludus::Engine::Graphics::Color Color = Ludus::Engine::Graphics::Colors::LightGray;
		Ludus::Engine::Graphics::HorizontalTextAlignment HorizontalTextAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Center;
	};
}
