#include <Ludus/Engine/Random.h>
#include <Ludus/Engine/TimeStep.h>
#include <Ludus/Engine/Utilities.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Math/Numeric.h>
#include <Ludus/Math/Rectangle2D.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Key.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

using Ludus::Math::Vector2D;
using Ludus::Math::Transform2D;
using Ludus::Math::Rectangle2D;
using Ludus::Graphics::Camera2D;
using Ludus::Graphics::Renderer2D;
using Ludus::Graphics::Shader;
using Ludus::Graphics::GLContext;
using Ludus::Graphics::Color;
using Ludus::Platform::Input;
using Ludus::Platform::Key;
using Ludus::Platform::Window;
using Ludus::Platform::WindowOptions;

namespace Colors = Ludus::Graphics::Colors;
namespace Numeric = Ludus::Math::Numeric;

enum GameState
{
	Menu, Playing, Paused, Score
};

const int Width = 800;
const int Height = 640;

const float BallSize = 10.0f;
const float BallSpeedDefault = 600.0f;
const float BallSpeedIncrement = 20.0f;
const float PaddleRightSpeed = 400;
const float PaddleLeftSpeed = 600.0f;
const int MaxScore = 3;

const int PaddleScoreTextSize = 50;
const int PaddleScoreTextOffset = 75;
const float PaddleWidth = 10.0f;
const float PaddleHeight = 40.0f;
const float PaddleWidthOffset = 50;
const float PaddleHeightOffset = PaddleHeight / 2.0f;

Rectangle2D LeftScoreTextRect;
Rectangle2D RightScoreTextRect;
Rectangle2D PaddleLeftRect;
Rectangle2D PaddleRightRect;
Rectangle2D BallRect;

Ludus::Engine::Random random;
Ludus::Engine::TimeStep Timer;

Vector2D PaddleLeftCenter;
Vector2D PaddleLeftDirection;

Vector2D PaddleRightCenter;
Vector2D PaddleRightDirection;

Vector2D BallCenter;
Vector2D BallDirection;

GameState State = Menu;
int MenuIndex = 1;

float BallSpeed = BallSpeedDefault;

int PaddleLeftScore = 0;
int PaddleRightScore = 0;

bool IsBallServed = false;
bool isMultiplayer = false;
bool IsRunning = false;


#pragma region State Helpers

void static Clear()
{
	PaddleLeftCenter = Vector2D(PaddleWidthOffset, Height / 2.0f);
	PaddleLeftDirection = Vector2D::Zero();

	PaddleRightCenter = Vector2D(Width - PaddleWidthOffset, Height / 2.0f);
	PaddleRightDirection = Vector2D::Zero();

	BallCenter = Vector2D(Width / 2.0f, Height / 2.0f);
	BallDirection = Vector2D::Zero();

	BallSpeed = BallSpeedDefault;

	IsRunning = false;
}

void static Start()
{
	auto directionX = random.Next(-1.0f, 1.0f);
	directionX = directionX < 0.0f ? -1.0f : 1.0f;
	auto directionY = random.Next(-0.5f, 0.5f);

	BallDirection = Vector2D(directionX, directionY);
	BallDirection.Normalize();

	IsBallServed = false;
	IsRunning = true;
}

void static UpdateRects()
{
	PaddleLeftRect = Rectangle2D(PaddleLeftCenter.X - PaddleWidth / 2.0f,
		PaddleLeftCenter.Y - PaddleHeight / 2.0f,
		PaddleWidth, PaddleHeight);

	PaddleRightRect = Rectangle2D(PaddleRightCenter.X - PaddleWidth / 2.0f,
		PaddleRightCenter.Y - PaddleHeight / 2.0f,
		PaddleWidth, PaddleHeight);

	BallRect = Rectangle2D(BallCenter.X - BallSize / 2.0f,
		BallCenter.Y - BallSize / 2.0f,
		BallSize, BallSize);
}

#pragma endregion

#pragma region Rendering Helpers

void static RenderGame(Renderer2D& renderer)
{
	// Render stippled center line.
	auto numLines = 40;
	auto lineHeight = Height / numLines;
	auto xCenter = Width / 2.0f;

	for (int i = 0; i < numLines; i += 2)
	{
		auto padding = lineHeight / 2.0f;
		auto y1 = lineHeight * i + padding;
		auto y2 = lineHeight * i + lineHeight + padding;
		renderer.DrawLine(xCenter, y1, xCenter, y2, Colors::White);
	}

	// Render scores
	LeftScoreTextRect = Rectangle2D(xCenter - PaddleScoreTextOffset - PaddleScoreTextSize / 2.0f, Height - PaddleScoreTextSize - 10.0f, 50.0f, 50.0);
	renderer.DrawText(Transform2D(0, Vector2D(LeftScoreTextRect.GetX(), LeftScoreTextRect.GetY())), std::to_string(PaddleLeftScore));

	RightScoreTextRect = Rectangle2D(xCenter + PaddleScoreTextOffset - PaddleScoreTextSize / 2.0f, Height - PaddleScoreTextSize - 10.0f, 50.0f, 50.0);
	renderer.DrawText(Transform2D(0, Vector2D(RightScoreTextRect.GetX(), RightScoreTextRect.GetY())), std::to_string(PaddleRightScore));

	// Render left paddle.
	renderer.DrawQuad(Transform2D(0, Vector2D(PaddleLeftRect.GetX(), PaddleLeftRect.GetY()), Vector2D(PaddleLeftRect.GetWidth(), PaddleLeftRect.GetHeight())), Colors::White);

	// Render right paddle.
	renderer.DrawQuad(Transform2D(0, Vector2D(PaddleRightRect.GetX(), PaddleRightRect.GetY()), Vector2D(PaddleRightRect.GetWidth(), PaddleRightRect.GetHeight())), Colors::White);

	// Render ball.
	renderer.DrawQuad(Transform2D(0, Vector2D(BallRect.GetX(), BallRect.GetY()), Vector2D(BallRect.GetWidth(), BallRect.GetHeight())), Colors::White);
}

void static RenderMenuScreen(Renderer2D& renderer)
{
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height - 150.0f), 3.0f), "Pong", Colors::White);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height / 2.0f)), "Single Player", MenuIndex == 1 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height / 2.0f - 100.0f)), "Multiplayer", MenuIndex == 2 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height / 2.0f - 200.0f)), "Exit", MenuIndex == 3 ? Colors::White : Colors::LightGray);
}

void static RenderPausedScreen(Renderer2D& renderer)
{
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height - 150.0f), 3.0f), "Pong", Colors::White);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 100.0f, Height / 2.0f)), "Continue", MenuIndex == 1 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 50.0f, Height / 2.0f - 100.0f)), "Exit", MenuIndex == 2 ? Colors::White : Colors::LightGray);
}

void static RenderScoreScreen(Renderer2D& renderer)
{
	std::string winnerName = PaddleLeftScore > PaddleRightScore ? "Left player" : "Right Player";
	std::string scoreText = winnerName + " Won!";

	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 200.0f, Height - 150.0f)), scoreText, Colors::White);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 125.0f, Height / 2.0f)), "New Game", MenuIndex == 1 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 50.0f, Height / 2.0f - 100.0f)), "Exit", MenuIndex == 2 ? Colors::White : Colors::LightGray);
}

#pragma endregion

int main()
{

#pragma region Initialization

	auto windowOptions = WindowOptions(Width, Height, "Pong (1972)", false);
	auto window = Window(windowOptions);

	GLContext::Init();
	GLContext::EnableBlending();
	GLContext::SetBlendAlpha();

	// Create Shader.
	Shader shader("Resources/Shaders");

	// Create Camera.
	Camera2D camera;
	camera.SetViewport(Width, Height);
	camera.SetPosition({ 0.0f, 0.0f });
	camera.SetZoom(1.0f);
	camera.SetRotation(0.0f);

	// Create Renderer.
	Renderer2D renderer(shader);
	renderer.SetClearColor(Colors::Black);

	Clear();

#pragma endregion

	while (!window.WindowShouldClose())
	{

#pragma region Input Handling

		if (State == Menu)
		{
			if (window.GetInput().GetKeyDown(Key::Up))
			{
				MenuIndex = MenuIndex == 1 ? 3 : MenuIndex - 1;
			}

			if (window.GetInput().GetKeyDown(Key::Down))
			{
				MenuIndex = MenuIndex == 3 ? 1 : MenuIndex + 1;
			}

			if (window.GetInput().GetKeyDown(Key::Enter) || window.GetInput().GetKeyDown(Key::Space))
			{
				if (MenuIndex == 1)
				{
					State = Playing;
					isMultiplayer = false;
				}
				else if (MenuIndex == 2)
				{
					State = Playing;
					isMultiplayer = true;
				}
				else
				{
					window.SetWindowShouldClose();
				}
			}
		}

		if (State == Playing)
		{
			if (PaddleLeftScore == MaxScore || PaddleRightScore == MaxScore)
			{
				Clear();

				State = Score;
				MenuIndex = 1;
			}

			if (!IsRunning && (window.GetInput().GetKeyDown(Key::Enter) || window.GetInput().GetKeyDown(Key::Space)))
			{
				Start();
			}

			if (window.GetInput().GetKeyDown(Key::Escape))
			{
				State = Paused;
				MenuIndex = 1;
			}
		}

		if (State == Paused)
		{
			if (window.GetInput().GetKeyDown(Key::Enter) || window.GetInput().GetKeyDown(Key::Space))
			{
				if (MenuIndex == 1)
				{
					State = Playing;
				}
				else
				{
					window.SetWindowShouldClose();
				}
			}

			if (window.GetInput().GetKeyDown(Key::Up) || window.GetInput().GetKeyDown(Key::Down))
			{
				MenuIndex = MenuIndex == 1 ? 2 : 1;
			}
		}

		if (State == Score)
		{
			if (window.GetInput().GetKeyDown(Key::Enter) || window.GetInput().GetKeyDown(Key::Space))
			{
				if (MenuIndex == 1)
				{
					Clear();

					PaddleLeftScore = PaddleRightScore = 0;

					State = Playing;
				}
				else
				{
					window.SetWindowShouldClose();
				}
			}

			if (window.GetInput().GetKeyDown(Key::Up) || window.GetInput().GetKeyDown(Key::Down))
			{
				MenuIndex = MenuIndex == 1 ? 2 : 1;
			}
		}

#pragma endregion

#pragma region Movement Integration

		// TODO: Remove input references from the simulation region. It should only apply the result of already chosen inputs.
		if (State == Playing)
		{
			float leftPaddleVerticalDirection = 0.0f;
			if (window.GetInput().GetKey(Key::W))
			{
				if (PaddleLeftCenter.Y <= Height - PaddleHeightOffset)
				{
					leftPaddleVerticalDirection = 1.0f;
				}
			}
			else if (window.GetInput().GetKey(Key::S))
			{
				if (PaddleLeftCenter.Y >= 0.0f + PaddleHeightOffset)
				{
					leftPaddleVerticalDirection = -1.0f;
				}
			}

			PaddleLeftDirection.Y = leftPaddleVerticalDirection;
			PaddleLeftDirection.Normalize();
			auto paddleLeftVelocity = PaddleLeftDirection * PaddleLeftSpeed;
			PaddleLeftCenter += paddleLeftVelocity * Timer;

			float rightPaddleVerticalDirection = 0.0f;
			if (isMultiplayer)
			{
				if (window.GetInput().GetKey(Key::Up))
				{
					if (PaddleRightCenter.Y <= Height - PaddleHeightOffset)
					{
						rightPaddleVerticalDirection = 1.0f;
					}
				}
				else if (window.GetInput().GetKey(Key::Down))
				{
					if (PaddleRightCenter.Y >= 0.0f + PaddleHeightOffset)
					{
						rightPaddleVerticalDirection = -1.0f;
					}
				}
			}
			else
			{
				if (IsRunning && abs(BallCenter.Y - PaddleRightCenter.Y) > BallSize / 2.0f && BallCenter.X >= Width / 2.0f)
				{
					if (PaddleRightCenter.Y < BallCenter.Y && PaddleRightCenter.Y <= Height - PaddleHeightOffset)
					{
						rightPaddleVerticalDirection = 1.0f;
					}
					else if (PaddleRightCenter.Y >= BallCenter.Y && PaddleRightCenter.Y >= 0.0f + PaddleHeightOffset)
					{
						rightPaddleVerticalDirection = -1.0f;
					}
				}
			}

			PaddleRightDirection.Y = rightPaddleVerticalDirection;
			PaddleRightDirection.Normalize();
			auto paddleRightVelocity = PaddleRightDirection * PaddleRightSpeed;
			PaddleRightCenter += paddleRightVelocity * Timer;

			auto ballVelocity = BallDirection * (IsBallServed ? BallSpeed : BallSpeed / 2.0f);
			BallCenter += ballVelocity * Timer;
		}

		UpdateRects();

#pragma endregion

#pragma region Collision Handling

		if (State == Playing)
		{
			if (BallRect.GetX() <= 0.0f)
			{
				PaddleRightScore++;
				Clear();
			}
			if (BallCenter.X + BallRect.GetWidth() >= Width)
			{
				PaddleLeftScore++;
				Clear();
			}

			if (PaddleLeftRect.Intersects(BallRect))
			{
				auto collisionNormal = Vector2D(1.0f, 0.0f);
				if (Vector2D::Dot(BallDirection, collisionNormal) < 0.0f)
				{
					auto collisionOffset = Numeric::Clamp((BallCenter.Y - PaddleLeftCenter.Y) / (PaddleHeight / 2.0f), -1.0f, 1.0f);
					auto tangentialKick = 0.75f;

					BallDirection.Reflect(collisionNormal).Normalize();
					BallDirection.Y += collisionOffset * tangentialKick;
					BallDirection.Normalize();
					BallSpeed += BallSpeedIncrement;
				}

				IsBallServed = true;
			}

			if (PaddleRightRect.Intersects(BallRect))
			{
				auto collisionNormal = Vector2D(-1.0f, 0.0f);
				if (Vector2D::Dot(BallDirection, collisionNormal) < 0.0f)
				{
					auto collisionOffset = Numeric::Clamp((BallCenter.Y - PaddleRightCenter.Y) / (PaddleHeight / 2.0f), -1.0f, 1.0f);
					auto tangentialKick = 0.75f;

					BallDirection.Reflect(collisionNormal).Normalize();
					BallDirection.Y += collisionOffset * tangentialKick;
					BallDirection.Normalize();
					BallSpeed += BallSpeedIncrement;
				}

				IsBallServed = true;
			}

			if (BallRect.GetY() + BallRect.GetHeight() >= Height)
			{
				BallDirection.Reflect(Vector2D(0.0f, -1.0f)).Normalize();
			}

			if (BallRect.GetY() <= 0.0f)
			{
				BallDirection.Reflect(Vector2D(0.0f, 1.0f)).Normalize();
			}
		}

#pragma endregion

#pragma region Rendering

		renderer.BeginScene(camera);
		renderer.Clear();

		if (State == Menu)
		{
			RenderMenuScreen(renderer);
		}

		if (State == Playing)
		{
			RenderGame(renderer);
		}

		if (State == Paused)
		{
			RenderPausedScreen(renderer);
		}

		if (State == Score)
		{
			RenderScoreScreen(renderer);
		}

		renderer.EndScene();

#pragma endregion

		window.SwapBuffers();
		window.PollEvents();

		Timer.Step();
	}
}
