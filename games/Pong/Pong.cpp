#include <core/Random.h>
#include <core/TimeStep.h>
#include <graphics/Camera2D.h>
#include <graphics/GLContext.h>
#include <graphics/Renderer2D.h>
#include <graphics/Shader.h>
#include <math/Numeric.h>
#include <math/Vector2D.h>
#include <physics/Rectangle2D.h>
#include <windowing/Window.h>
#include <windowing/WindowOptions.h>

enum GameState
{
	Menu, Playing, Paused, Score
};

GameState State = Menu;
int MenuIndex = 1;

int Width = 800;
int Height = 640;

Random random;
TimeStep Timer;

int PaddleLeftScore = 0;
int PaddleRightScore = 0;
int MaxScore = 3;
int PaddleScoreTextSize = 50;
int PaddleScoreTextOffset = 75;

Vector2D PaddleLeftCenter;
Vector2D PaddleLeftDirection;
float PaddleLeftSpeed = 600.0f;

Vector2D PaddleRightCenter;
Vector2D PaddleRightDirection;
float PaddleRightSpeed = 400;

Vector2D BallCenter;
Vector2D BallDirection;
float BallSpeed = 600.0f;
float BallSpeedDefault = BallSpeed;
float BallSpeedIncrement = 20.0f;
float BallSize = 10.0f;
bool IsBallServed = false;

float PaddleWidth = 10.0f;
float PaddleHeight = 40.0f;
float PaddleWidthOffset = 50;
float PaddleHeightOffset = PaddleHeight / 2.0f;

Rectangle2D LeftScoreTextRect;
Rectangle2D RightScoreTextRect;
Rectangle2D PaddleLeftRect;
Rectangle2D PaddleRightRect;
Rectangle2D BallRect;

bool isMultiplayer = false;
bool IsRunning = false;

#pragma region State helpers

void Clear()
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

void Start()
{
	auto directionX = random.Next(-1.0f, 1.0f);
	directionX = directionX < 0.0f ? -1.0f : 1.0f;
	auto directionY = random.Next(-0.5f, 0.5f);

	BallDirection = Vector2D(directionX, directionY);
	BallDirection.Normalize();

	IsBallServed = false;
	IsRunning = true;
}

void UpdateRects()
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

#pragma region Rendering helpers

void RenderGame(Renderer2D& renderer)
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
	renderer.DrawText(Transform2D(0, Vector2D(LeftScoreTextRect.GetX(), LeftScoreTextRect.GetY())), Colors::White, std::to_string(PaddleLeftScore));

	RightScoreTextRect = Rectangle2D(xCenter + PaddleScoreTextOffset - PaddleScoreTextSize / 2.0f, Height - PaddleScoreTextSize - 10.0f, 50.0f, 50.0);
	renderer.DrawText(Transform2D(0, Vector2D(RightScoreTextRect.GetX(), RightScoreTextRect.GetY())), Colors::White, std::to_string(PaddleRightScore));

	// Render left paddle.
	renderer.DrawQuad(Transform2D(0, Vector2D(PaddleLeftRect.GetX(), PaddleLeftRect.GetY()), Vector2D(PaddleLeftRect.GetWidth(), PaddleLeftRect.GetHeight())), Colors::White);

	// Render right paddle.
	renderer.DrawQuad(Transform2D(0, Vector2D(PaddleRightRect.GetX(), PaddleRightRect.GetY()), Vector2D(PaddleRightRect.GetWidth(), PaddleRightRect.GetHeight())), Colors::White);

	// Render ball.
	renderer.DrawQuad(Transform2D(0, Vector2D(BallRect.GetX(), BallRect.GetY()), Vector2D(BallRect.GetWidth(), BallRect.GetHeight())), Colors::White);
}

void RenderMenuScreen(Renderer2D& renderer)
{
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height - 150.0f), 3.0f), Colors::White, "Pong");
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height / 2.0f)), MenuIndex == 1 ? Colors::White : Colors::LightGray, "Single Player");
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height / 2.0f - 100.0f)), MenuIndex == 2 ? Colors::White : Colors::LightGray, "Multiplayer");
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height / 2.0f - 200.0f)), MenuIndex == 3 ? Colors::White : Colors::LightGray, "Exit");
}

void RenderPausedScreen(Renderer2D& renderer)
{
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 150.0f, Height - 150.0f), 3.0f), Colors::White, "Pong");
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 100.0f, Height / 2.0f)), MenuIndex == 1 ? Colors::White : Colors::LightGray, "Continue");
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 50.0f, Height / 2.0f - 100.0f)), MenuIndex == 2 ? Colors::White : Colors::LightGray, "Exit");
}

void RenderScoreScreen(Renderer2D& renderer)
{
	std::string winnerName = PaddleLeftScore > PaddleRightScore ? "Left player" : "Right Player";
	std::string scoreText = winnerName + " Won!";

	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 200.0f, Height - 150.0f)), Colors::White, scoreText);
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 125.0f, Height / 2.0f)), MenuIndex == 1 ? Colors::White : Colors::LightGray, "New Game");
	renderer.DrawText(Transform2D(0, Vector2D(Width / 2.0f - 50.0f, Height / 2.0f - 100.0f)), MenuIndex == 2 ? Colors::White : Colors::LightGray, "Exit");
}

#pragma endregion

int main()
{

#pragma region Startup

	auto windowOptions = WindowOptions(Width, Height, "Pong (1972)", false);
	auto window = Window(windowOptions);

	GLContext::Init();
	GLContext::EnableBlending();
	GLContext::SetBlendAlpha();

	// Create Shader.
	Shader shader("resources/shaders");

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

#pragma region Simulation

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
