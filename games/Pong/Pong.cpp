#include <vector>

#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/Random.h>
#include <Ludus/Engine/TimeStep.h>
#include <Ludus/Engine/TransformRegistry.h>
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

using Ludus::Engine::GameObject;
using Ludus::Engine::GameObjectHandle;
using Ludus::Graphics::Camera2D;
using Ludus::Graphics::Color;
using Ludus::Graphics::GLContext;
using Ludus::Graphics::Renderer2D;
using Ludus::Graphics::Shader;
using Ludus::Math::Vector2D;
using Ludus::Math::Transform2D;
using Ludus::Math::Rectangle2D;
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

const int HalfWidth = Width * 0.5f;
const int HalfHeight = Height * 0.5f;

const float PaddleWidth = 10.0f;
const float PaddleHeight = 40.0f;
const float PaddleWidthOffset = 50;
const float HalfPaddleHeight = PaddleHeight * 0.5f;
const float WallThickness = 20.0f;
const int ScoreTextOffset = 75;
const int NumLines = 25;
const int LineHeight = Height / NumLines;
const int HalfLineHeight = LineHeight * 0.5f;

const float BallSize = 10.0f;
const float BallSpeedDefault = 600.0f;
const float BallSpeedIncrement = 20.0f;
const float Player1Speed = 600.0f;
const float Player2Speed = 500.0f;
const int MaxScore = 5;

Ludus::Engine::Random random;
Ludus::Engine::TimeStep Timer;
Ludus::Engine::TransformRegistry transformRegistry;

std::vector<GameObject> GameObjects;

Rectangle2D LeftScoreTextRect;
Rectangle2D RightScoreTextRect;
Rectangle2D Player1Rect;
Rectangle2D Player2Rect;
Rectangle2D BallRect;

GameObjectHandle Player1Handle;
GameObjectHandle Player2Handle;
GameObjectHandle BallHandle;

GameObjectHandle BoundaryLeftHandle;
GameObjectHandle BoundaryTopHandle;
GameObjectHandle BoundaryRightHandle;
GameObjectHandle BoundaryBottomHandle;

Vector2D BallDirection;

GameState State = Menu;
int MenuIndex = 1;

float BallSpeed = BallSpeedDefault;

int Player1Score = 0;
int Player2Score = 0;

bool IsBallServed = false;
bool IsMultiplayer = false;
bool IsRunning = false;


#pragma region State Helpers

void static Clear()
{
	auto ballTransform = transformRegistry.TryGetByOwnerMutable(BallHandle);
	if (ballTransform)
	{
		ballTransform->Position = { HalfWidth, HalfHeight };
	}

	auto player1Transform = transformRegistry.TryGetByOwnerMutable(Player1Handle);
	if (player1Transform)
	{
		player1Transform->Position = { PaddleWidthOffset, HalfHeight };
	}

	auto player2Transform = transformRegistry.TryGetByOwnerMutable(Player2Handle);
	if (player2Transform)
	{
		player2Transform->Position = { Width - PaddleWidthOffset, HalfHeight };
	}

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

#pragma endregion

#pragma region Rendering Helpers

void static RenderGame(Renderer2D& renderer)
{
	// Render stippled center line.
	for (int y = 0; y < Height; y += LineHeight)
	{
		renderer.DrawLine(HalfWidth, y, HalfWidth, y + HalfLineHeight, Colors::White);
	}

	// Render score text.
	renderer.DrawText(Transform2D(0, { HalfWidth - ScoreTextOffset - 25.0f, Height - ScoreTextOffset }), std::to_string(Player1Score));
	renderer.DrawText(Transform2D(0, { HalfWidth + ScoreTextOffset, Height - ScoreTextOffset }), std::to_string(Player2Score));

	auto* player1Ptr = transformRegistry.TryGetByOwnerMutable(Player1Handle);
	auto* player2Ptr = transformRegistry.TryGetByOwnerMutable(Player2Handle);
	auto* ballPtr = transformRegistry.TryGetByOwnerMutable(BallHandle);

	if (!(player1Ptr && player2Ptr && ballPtr))
	{
		Ludus::Engine::Utilities::WriteLine(
			"[Rendering] An error occurred while attempting to get object transforms."
		);
		return;
	}

	// Render ball and players.
	renderer.DrawQuad(*player1Ptr);
	renderer.DrawQuad(*player2Ptr);
	renderer.DrawQuad(*ballPtr);
}

void static RenderMenuScreen(Renderer2D& renderer)
{
	renderer.DrawText(Transform2D(0, { HalfWidth - 175.0f, Height - 150.0f }, 3.0f), "Pong");
	renderer.DrawText(Transform2D(0, { HalfWidth - 150.0f, HalfHeight }), "Single Player", MenuIndex == 1 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, { HalfWidth - 120.0f, HalfHeight - 100.0f }), "Multiplayer", MenuIndex == 2 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, { HalfWidth - 60.0f, HalfHeight - 200.0f }), "Exit", MenuIndex == 3 ? Colors::White : Colors::LightGray);
}

void static RenderPausedScreen(Renderer2D& renderer)
{
	renderer.DrawText(Transform2D(0, { HalfWidth - 175.0f, Height - 150.0f }, 3.0f), "Pong");
	renderer.DrawText(Transform2D(0, { HalfWidth - 80.0f, HalfHeight }), "Continue", MenuIndex == 1 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, { HalfWidth - 30.0f, HalfHeight - 100.0f }), "Exit", MenuIndex == 2 ? Colors::White : Colors::LightGray);
}

void static RenderScoreScreen(Renderer2D& renderer)
{
	std::string winnerName = Player1Score > Player2Score ? "Player 1" : "Player 2";
	std::string scoreText = winnerName + " Won!";

	renderer.DrawText(Transform2D(0, { HalfWidth - 200.0f, Height - 150.0f }), scoreText);
	renderer.DrawText(Transform2D(0, { HalfWidth - 125.0f, HalfHeight }), "New Game", MenuIndex == 1 ? Colors::White : Colors::LightGray);
	renderer.DrawText(Transform2D(0, { HalfWidth - 50.0f, HalfHeight - 100.0f }), "Exit", MenuIndex == 2 ? Colors::White : Colors::LightGray);
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

#pragma region Game objects setup

	// Game objects.
	GameObject ballObject;
	GameObject boundaryLeftObject;
	GameObject boundaryTopObject;
	GameObject boundaryRightObject;
	GameObject boundaryBottomObject;
	GameObject player1Object;
	GameObject player2Object;

	BallHandle = ballObject.Handle;
	BoundaryLeftHandle = boundaryLeftObject.Handle;
	BoundaryTopHandle = boundaryTopObject.Handle;
	BoundaryRightHandle = boundaryRightObject.Handle;
	BoundaryBottomHandle = boundaryBottomObject.Handle;
	Player1Handle = player1Object.Handle;
	Player2Handle = player2Object.Handle;

	GameObjects.push_back(player1Object);
	GameObjects.push_back(player2Object);
	GameObjects.push_back(ballObject);
	GameObjects.push_back(boundaryLeftObject);
	GameObjects.push_back(boundaryTopObject);
	GameObjects.push_back(boundaryRightObject);
	GameObjects.push_back(boundaryBottomObject);

	// Transforms.
	transformRegistry.Add(BallHandle, { HalfWidth, HalfHeight }, BallSize);
	transformRegistry.Add(BoundaryLeftHandle, { WallThickness * 0.5f, HalfHeight }, { WallThickness, Height });
	transformRegistry.Add(BoundaryTopHandle, { HalfWidth, Height - WallThickness * 0.5f }, { Width, WallThickness });
	transformRegistry.Add(BoundaryRightHandle, { Width - WallThickness * 0.5f, HalfHeight }, { WallThickness, Height });
	transformRegistry.Add(BoundaryBottomHandle, { HalfWidth, WallThickness * 0.5f }, { Width, WallThickness });
	transformRegistry.Add(Player1Handle, { PaddleWidthOffset, HalfHeight }, { PaddleWidth, PaddleHeight });
	transformRegistry.Add(Player2Handle, { Width - PaddleWidthOffset, HalfHeight }, { PaddleWidth, PaddleHeight });

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
					IsMultiplayer = false;

					Clear();
				}
				else if (MenuIndex == 2)
				{
					State = Playing;
					IsMultiplayer = true;

					Clear();
				}
				else
				{
					window.SetWindowShouldClose();
				}
			}
		}

		if (State == Playing)
		{
			if (Player1Score == MaxScore || Player2Score == MaxScore)
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

					Player1Score = Player2Score = 0;

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

		if (State == Playing)
		{
			auto* player1Ptr = transformRegistry.TryGetByOwnerMutable(Player1Handle);
			auto* player2Ptr = transformRegistry.TryGetByOwnerMutable(Player2Handle);
			auto* ballPtr = transformRegistry.TryGetByOwnerMutable(BallHandle);

			if (!(player1Ptr && player2Ptr && ballPtr))
			{
				Ludus::Engine::Utilities::WriteLine(
					"[Movement Integration] An error occurred while attempting to get object transforms."
				);
				continue;
			}

			auto& player1Transform = *player1Ptr;
			auto& player2Transform = *player2Ptr;
			auto& ballTransform = *ballPtr;

			// Player 1.
			auto player1Direction = 0.0f;
			if (window.GetInput().GetKey(Key::W) && player1Transform.Position.Y + player1Transform.Scale.Y * 0.5f <= Height)
			{
				player1Direction = 1.0f;
			}
			else if (window.GetInput().GetKey(Key::S) && player1Transform.Position.Y - player1Transform.Scale.Y * 0.5f >= 0.0f)
			{
				player1Direction = -1.0f;
			}

			player1Transform.Position.Y += player1Direction * Player1Speed * Timer;

			// Player 2.
			auto player2Direction = 0.0f;
			if (IsMultiplayer)
			{
				if (window.GetInput().GetKey(Key::Up) && player2Transform.Position.Y + player2Transform.Scale.Y * 0.5f <= Height)
				{
					player2Direction = 1.0f;
				}
				else if (window.GetInput().GetKey(Key::Down) && player2Transform.Position.Y - player2Transform.Scale.Y * 0.5f >= 0.0f)
				{
					player2Direction = -1.0f;
				}
			}
			else
			{
				if (IsRunning && abs(ballTransform.Position.Y - player2Transform.Position.Y) > BallSize && ballTransform.Position.X >= HalfWidth)
				{
					if (player2Transform.Position.Y < ballTransform.Position.Y && player2Transform.Position.Y + player2Transform.Scale.Y * 0.5f <= Height)
					{
						player2Direction = 1.0f;
					}
					else if (player2Transform.Position.Y >= ballTransform.Position.Y && player2Transform.Position.Y - player2Transform.Scale.Y * 0.5f >= 0.0f)
					{
						player2Direction = -1.0f;
					}
				}
			}

			player2Transform.Position.Y += player2Direction * Player2Speed * Timer;

			// Ball.
			auto ballVelocity = BallDirection * (IsBallServed ? BallSpeed : BallSpeed * 0.5f);
			ballTransform.Position += ballVelocity * Timer;
		}

#pragma endregion

#pragma region Collision Handling

		if (State == Playing)
		{
			auto* player1Ptr = transformRegistry.TryGetByOwnerMutable(Player1Handle);
			auto* player2Ptr = transformRegistry.TryGetByOwnerMutable(Player2Handle);
			auto* ballPtr = transformRegistry.TryGetByOwnerMutable(BallHandle);

			if (!(player1Ptr && player2Ptr && ballPtr))
			{
				Ludus::Engine::Utilities::WriteLine(
					"[Collision Handling] An error occurred while attempting to get object transforms."
				);
				continue;
			}

			auto& player1Transform = *player1Ptr;
			auto& player2Transform = *player2Ptr;
			auto& ballTransform = *ballPtr;

			if (ballTransform.Position.X - BallSize * 0.5f <= 0.0f)
			{
				Player2Score++;
				Clear();
			}
			if (ballTransform.Position.X + BallSize * 0.5f >= Width)
			{
				Player1Score++;
				Clear();
			}

			auto player1Rect = Rectangle2D(player1Transform.Position - player1Transform.Scale * 0.5f, player1Transform.Scale);
			auto player2Rect = Rectangle2D(player2Transform.Position - player2Transform.Scale * 0.5f, player2Transform.Scale);
			auto ballRect = Rectangle2D(ballTransform.Position - ballTransform.Scale * 0.5f, ballTransform.Scale);

			if (player1Rect.Intersects(ballRect))
			{
				auto collisionNormal = Vector2D(1.0f, 0.0f);
				if (Vector2D::Dot(BallDirection, collisionNormal) < 0.0f)
				{
					auto collisionOffset = Numeric::Clamp((ballTransform.Position.Y - player1Transform.Position.Y) / HalfPaddleHeight, -1.0f, 1.0f);
					auto tangentialKick = 0.75f;

					BallDirection.Reflect(collisionNormal).Normalize();
					BallDirection.Y += collisionOffset * tangentialKick;
					BallDirection.Normalize();
					BallSpeed += BallSpeedIncrement;
				}

				IsBallServed = true;
			}

			if (player2Rect.Intersects(ballRect))
			{
				auto collisionNormal = Vector2D(-1.0f, 0.0f);
				if (Vector2D::Dot(BallDirection, collisionNormal) < 0.0f)
				{
					auto collisionOffset = Numeric::Clamp((ballTransform.Position.Y - player2Transform.Position.Y) / HalfPaddleHeight, -1.0f, 1.0f);
					auto tangentialKick = 0.75f;

					BallDirection.Reflect(collisionNormal).Normalize();
					BallDirection.Y += collisionOffset * tangentialKick;
					BallDirection.Normalize();
					BallSpeed += BallSpeedIncrement;
				}

				IsBallServed = true;
			}

			if (ballTransform.Position.Y + player1Transform.Scale.Y * 0.5f >= Height)
			{
				BallDirection.Reflect(Vector2D(0.0f, -1.0f)).Normalize();
			}

			if (ballTransform.Position.Y - player1Transform.Scale.Y * 0.5f <= 0.0f)
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
