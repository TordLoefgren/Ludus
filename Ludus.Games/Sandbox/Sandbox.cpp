#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Graphics/Texture.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Platform/Key.h>
#include <Ludus/Platform/Window.h>

using Ludus::Graphics::GLContext;
using Ludus::Graphics::Texture;
using Ludus::Math::Transform2D;
using Ludus::Math::Vector2D;
using Ludus::Platform::Key;

namespace Colors = Ludus::Graphics::Colors;

int Width = 800;
int Height = 640;


int main()
{
	auto window = Ludus::Platform::Window(800, 640, "Sandbox");

	GLContext::Init();
	GLContext::EnableBlending();
	GLContext::SetBlendAlpha();

	// Create Shader.
	Ludus::Graphics::Shader shader("Resources/Shaders");

	Texture texture1 = Texture::FromFile("Assets/Textures/WallTextureBrick1Light.png");
	Texture texture2 = Texture::FromFile("Assets/Textures/WallTextureBrick2Light.png");

	// Create Camera.
	Ludus::Graphics::Camera2D camera;

	float x = 0.0f, y = 0.0f;
	float zoom(1.0f);
	float rotation(0.0f);

	camera.SetViewport(Width, Height);
	camera.SetPosition({ x, y });
	camera.SetZoom(zoom);
	camera.SetRotation(rotation);

	// Create Renderer.
	Ludus::Graphics::Renderer2D renderer(shader);
	renderer.SetClearColor(Colors::DarkGray);

	while (!window.WindowShouldClose())
	{
		renderer.BeginScene(camera);
		renderer.Clear();

		renderer.DrawQuad(Transform2D(0, Vector2D({ Width * 0.25f, Height * 0.2f }), Vector2D(150.0f), 45.0f), Colors::White, &texture1);
		renderer.DrawQuad(Transform2D(0, Vector2D({ Width * 0.75f, Height * 0.2f }), Vector2D(150.0f)), Colors::Orange, &texture2);

		renderer.DrawQuad(Transform2D(0, Vector2D({ Width * 0.25f, Height * 0.5f }), Vector2D(150.0f, 100.0f)), Colors::Red);
		renderer.DrawCircle(Transform2D(0, Vector2D({ Width * 0.75f, Height * 0.5f }), Vector2D(150.0f)), Colors::Green);
		renderer.DrawLine(Width * 0.25f - 50, Height * 0.7f, Width * 0.25f + 50, Height * 0.7f, Colors::Blue);
		renderer.SetLineWidth(20.0f);
		renderer.DrawText(Transform2D(0, Vector2D({ Width * 0.50f, Height * 0.7f }), Vector2D(2.0f)), "Ludus");

		renderer.EndScene();

		if (window.GetInput().GetKey(Key::A))
		{
			x -= 5.0f;
		}
		if (window.GetInput().GetKey(Key::D))
		{
			x += 5.0f;
		}
		if (window.GetInput().GetKey(Key::W))
		{
			y += 5.0f;
		}
		if (window.GetInput().GetKey(Key::S))
		{
			y -= 5.0f;
		}

		if (window.GetInput().GetKey(Key::Escape))
		{
			window.SetWindowShouldClose();
		}

		camera.SetPosition({ x, y });
		camera.SetZoom(zoom);
		camera.SetRotation(rotation);

		window.SwapBuffers();
		window.PollEvents();
	}
}
