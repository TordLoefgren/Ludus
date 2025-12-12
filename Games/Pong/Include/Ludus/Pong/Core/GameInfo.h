#pragma once

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Engine/Platform/Input.h>
#include <Ludus/Engine/Platform/Window.h>

namespace Ludus::Pong::Core
{
	struct GameInfo
	{
		Ludus::Engine::Core::Random Random { };
		Ludus::Engine::Graphics::Camera2D Camera { };

		Ludus::Engine::Graphics::Color ActiveColor = Ludus::Engine::Graphics::Colors::White;
		Ludus::Engine::Graphics::Color InactiveColor = Ludus::Engine::Graphics::Colors::LightGray;
	};
}
