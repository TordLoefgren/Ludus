#pragma once

#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Engine/Random.h>
#include <Ludus/Engine/Time.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Physics/CollisionSystem2D.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

namespace Pong::Core
{
	struct GameInfo
	{
		Ludus::Platform::Window& Window;
		const Ludus::Platform::WindowOptions& WindowOptions;
		Ludus::Graphics::Shader Shader;
		Ludus::Graphics::Camera2D Camera;
		Ludus::Graphics::Renderer2D Renderer;
		Ludus::Physics::CollisionSystem2D CollisionSystem;
		Ludus::Engine::EntityComponentSystem EntityComponentSystem;
		Ludus::Engine::Time Timer;
		Ludus::Engine::Random Random;

		GameInfo(
			Ludus::Platform::Window& window,
			Ludus::Platform::WindowOptions& windowOptions
		) :
			Window(window),
			WindowOptions(windowOptions),
			Shader("Resources/Shaders"),
			Camera(),
			Renderer(Shader),
			CollisionSystem(),
			EntityComponentSystem(),
			Timer(),
			Random()
		{
			Camera.SetViewport(WindowOptions.Width, WindowOptions.Height);
			Camera.SetPosition({ 0.0f, 0.0f });
			Camera.SetZoom(1.0f);
			Camera.SetRotation(0.0f);

			Renderer.SetClearColor(Ludus::Graphics::Colors::Black);
		}
	};
}
