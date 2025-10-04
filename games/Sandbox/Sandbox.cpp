#include <graphics/Camera2D.h>
#include <graphics/GLContext.h>
#include <graphics/Renderer2D.h>
#include <graphics/Shader.h>
#include <windowing/Window.h>


int Width = 800;
int Height = 640;

int main()
{
	auto window = Window(800, 640, "Sandbox");

	GLContext::Init();
	GLContext::EnableBlending();
	GLContext::SetBlendAlpha();

	// Create Shader.
	Shader shader("resources/shaders");

	Texture texture1 = Texture::FromFile("assets/textures/WallTextureBrick1Light.png");
	Texture texture2 = Texture::FromFile("assets/textures/WallTextureBrick2Light.png");

	// Create Camera.
	Camera2D camera;

	float x = 0.0f, y = 0.0f;
	float zoom(1.0f);
	float rotation(0.0f);

	camera.SetViewport(Width, Height);
	camera.SetPosition({ x, y });
	camera.SetZoom(zoom);
	camera.SetRotation(rotation);

	// Create Renderer.
	Renderer2D renderer(shader);
	renderer.SetClearColor(Colors::DarkGray);

	while (!window.WindowShouldClose())
	{
		renderer.BeginScene(camera);
		renderer.Clear();

		renderer.DrawText(Width / 2.0f - 50, Height / 2.0f - 50, 1.0f, Colors::Red, "Hello");

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
