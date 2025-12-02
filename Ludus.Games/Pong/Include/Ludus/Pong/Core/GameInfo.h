#pragma once

#include <Ludus/Core/SystemContext.h>
#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Engine/Random.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Physics/Queries/IPhysicsQueryCache2D.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Window.h>

namespace Ludus::Pong::Core
{
	struct GameInfo
	{
		Ludus::Engine::Random Random;

		Ludus::Graphics::Color ActiveColor = Ludus::Graphics::Colors::White;
		Ludus::Graphics::Color InactiveColor = Ludus::Graphics::Colors::LightGray;
	};
}
