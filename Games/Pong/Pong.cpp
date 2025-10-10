#include <algorithm>
#include <cmath> 
#include <vector>

#include <Ludus/Engine/ColliderRegistry.h>
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
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Collider2D.h>
#include <Ludus/Physics/Collision2DManager.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Key.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

using Ludus::Engine::GameObject;
using Ludus::Engine::GameObjectHandle;
using Ludus::Engine::LayerMask;
using Ludus::Graphics::Camera2D;
using Ludus::Graphics::Color;
using Ludus::Graphics::GLContext;
using Ludus::Graphics::Renderer2D;
using Ludus::Graphics::Shader;
using Ludus::Math::Vector2D;
using Ludus::Math::Transform2D;
using Ludus::Physics::Collider2D;
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

const int Width = 1024;
const int Height = 768;

const float HalfWidth = Width * 0.5f;
const float HalfHeight = Height * 0.5f;

const int NumLines = 26;
const int LineHeight = Height / NumLines;
const int HalfLineHeight = (int)(LineHeight * 0.5f);

const float PaddleWidth = 10.0f;
const float PaddleHeight = 60.0f;
const float PaddleWidthOffset = 100.0f;
const float HalfPaddleHeight = PaddleHeight * 0.5f;
const float WallWidthThickness = PaddleWidthOffset - 50.0f;
const float WallHeightThickness = 15.0f;
const float ScoreTextOffset = 75;

const float BallSize = 12.0f;
const float BallSpeedDefault = 900.0f;
const float BallSpeedIncrement = 50.0f;
const float MaxDeflectDegrees = 60.0f;
const float MinCenterDegrees = 10.0f;
const float Player1Speed = 900.0f;
const float Player2Speed = 700.0f;
const int MaxScore = 5;

Ludus::Engine::ColliderRegistry colliderRegistry;
Ludus::Physics::Collision2DManager collisionManager;
Ludus::Engine::TransformRegistry transformRegistry;
Ludus::Engine::Random random;
Ludus::Engine::TimeStep Timer;

const std::string BallLayerName = "Ball";
const std::string BoundaryHorizontalLayerName = "BoundaryHorizontal";
const std::string BoundaryVerticalLayerName = "BoundaryVertical";
const std::string Player1LayerName = "Player1";
const std::string Player2LayerName = "Player2";

GameObjectHandle Player1Handle;
GameObjectHandle Player2Handle;
GameObjectHandle BallHandle;
GameObjectHandle BoundaryLeftHandle;
GameObjectHandle BoundaryTopHandle;
GameObjectHandle BoundaryRightHandle;
GameObjectHandle BoundaryBottomHandle;

GameState State = Menu;
int MenuIndex = 1;

float BallSpeed = BallSpeedDefault;

int Player1Score = 0;
int Player2Score = 0;

bool IsBallServed = false;
bool IsMultiplayer = false;
bool IsRunning = false;

#pragma region State Helpers

static bool Is(const LayerMask& a, const LayerMask& b) { return a == b; };
static bool IsPair(const LayerMask& a1, const LayerMask& b1, const LayerMask& a2, const LayerMask& b2)
{
	return (a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2);
};

void static Clear()
{
	if (auto ballTransform = transformRegistry.TryGetByOwnerMutable(BallHandle))
	{
		ballTransform->Position = { HalfWidth, HalfHeight };
	}

	if (auto player1Transform = transformRegistry.TryGetByOwnerMutable(Player1Handle))
	{
		player1Transform->Position = { PaddleWidthOffset, HalfHeight };
	}

	if (auto player2Transform = transformRegistry.TryGetByOwnerMutable(Player2Handle))
	{
		player2Transform->Position = { Width - PaddleWidthOffset, HalfHeight };
	}

	BallSpeed = BallSpeedDefault;

	IsRunning = false;
}

void static Start()
{
	auto directionX = random.Next(-1.0f, 1.0f);
	directionX = directionX < 0.0f ? -1.0f : 1.0f;
	auto directionY = random.Next(-0.5f, 0.5f);

	if (auto ballTransform = transformRegistry.TryGetByOwnerMutable(BallHandle))
	{
		ballTransform->Rotation = Numeric::RadiansToDegrees(std::atan2(directionY, directionX));
	}

	IsBallServed = false;
	IsRunning = true;
}

static float GetReflectionAngle(const Transform2D* ballTransform, const Transform2D* playerTransform, Vector2D normal)
{
	// Compute a signed offset in [-1,1] from the vertical paddle impact position.
	auto offset = (ballTransform->Position.Y - playerTransform->Position.Y) / (playerTransform->Scale.Y * 0.5f);
	offset = Numeric::Clamp(offset, -1.0f, 1.0f);

	// Reflect the direction off the surface normal.
	const auto direction = ballTransform->Forward();
	const auto reflection = Vector2D::Reflect(direction, normal);

	const float theta = std::copysign(
		std::max(MinCenterDegrees, std::fabs(offset) * MaxDeflectDegrees),
		(offset != 0.0f) ? offset : (direction.Y >= 0.0f ? 1.0f : -1.0f)
	);
	auto deflection = Vector2D::Rotate(reflection, theta);

	// Make sure that the ball will not deflect completely vertical.
	if (std::fabs(deflection.X) < 0.1f)
	{
		deflection.X = deflection.X < 0.0f ? -0.1f : 0.1f;
	}

	return Numeric::RotationDegreesFromDirection(deflection.X, deflection.Y);
}

#pragma endregion

#pragma region Rendering Helpers

void static RenderGame(Renderer2D& renderer)
{
	// Render stippled center line.
	for (float y = 0; y < Height; y += LineHeight)
	{
		renderer.DrawLine(HalfWidth, y, HalfWidth, y + HalfLineHeight, Colors::White);
	}

	// Render score text.
	renderer.DrawText(Transform2D(0, { HalfWidth - ScoreTextOffset - 25.0f, Height - ScoreTextOffset }), std::to_string(Player1Score));
	renderer.DrawText(Transform2D(0, { HalfWidth + ScoreTextOffset, Height - ScoreTextOffset }), std::to_string(Player2Score));

	// Render top and bottom boundaries.
	const auto* boundaryTopTransform = transformRegistry.TryGetByOwnerMutable(BoundaryTopHandle);
	const auto* boundaryBottomTransform = transformRegistry.TryGetByOwnerMutable(BoundaryBottomHandle);
	if (!(boundaryTopTransform && boundaryBottomTransform))
	{
		Ludus::Engine::Utilities::WriteLine("[Rendering] Missing transform(s).");
		return;
	}

	renderer.DrawQuad(*boundaryTopTransform);
	renderer.DrawQuad(*boundaryBottomTransform);

	const auto* player1Ptr = transformRegistry.TryGetByOwnerMutable(Player1Handle);
	const auto* player2Ptr = transformRegistry.TryGetByOwnerMutable(Player2Handle);
	const auto* ballPtr = transformRegistry.TryGetByOwnerMutable(BallHandle);

	if (!(player1Ptr && player2Ptr && ballPtr))
	{
		Ludus::Engine::Utilities::WriteLine("[Rendering] Missing transform(s).");
		return;
	}

	// Render ball and players.
	renderer.DrawQuad(*player1Ptr);
	renderer.DrawQuad(*player2Ptr);
	renderer.DrawCircle(*ballPtr);
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

	renderer.DrawText(Transform2D(0, { HalfWidth - 150.0f, Height - 150.0f }), scoreText);
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
	const GameObject ballObject;
	const GameObject boundaryLeftObject;
	const GameObject boundaryTopObject;
	const GameObject boundaryRightObject;
	const GameObject boundaryBottomObject;
	const GameObject player1Object;
	const GameObject player2Object;

	BallHandle = ballObject.Handle;
	BoundaryLeftHandle = boundaryLeftObject.Handle;
	BoundaryTopHandle = boundaryTopObject.Handle;
	BoundaryRightHandle = boundaryRightObject.Handle;
	BoundaryBottomHandle = boundaryBottomObject.Handle;
	Player1Handle = player1Object.Handle;
	Player2Handle = player2Object.Handle;

	// Layer masks.
	LayerMask::AddLayer(BallLayerName, 1);
	LayerMask::AddLayer(BoundaryHorizontalLayerName, 2);
	LayerMask::AddLayer(BoundaryVerticalLayerName, 3);
	LayerMask::AddLayer(Player1LayerName, 4);
	LayerMask::AddLayer(Player2LayerName, 5);

	// Colliders.
	colliderRegistry.Add(BallHandle, 1, LayerMask::GetMask({ BoundaryVerticalLayerName, BoundaryHorizontalLayerName, Player1LayerName, Player2LayerName }));
	colliderRegistry.Add(BoundaryLeftHandle, 3, LayerMask::GetMask({ BallLayerName, Player1LayerName, Player2LayerName }), true);
	colliderRegistry.Add(BoundaryTopHandle, 2, LayerMask::GetMask({ BallLayerName, Player1LayerName, Player2LayerName }), true);
	colliderRegistry.Add(BoundaryRightHandle, 3, LayerMask::GetMask({ BallLayerName, Player1LayerName, Player2LayerName }), true);
	colliderRegistry.Add(BoundaryBottomHandle, 2, LayerMask::GetMask({ BallLayerName, Player1LayerName, Player2LayerName }), true);
	colliderRegistry.Add(Player1Handle, 4, LayerMask::GetMask({ BallLayerName, BoundaryHorizontalLayerName }));
	colliderRegistry.Add(Player2Handle, 5, LayerMask::GetMask({ BallLayerName, BoundaryHorizontalLayerName }));

	// Transforms.
	transformRegistry.Add(BallHandle, { HalfWidth, HalfHeight }, BallSize);
	transformRegistry.Add(BoundaryLeftHandle, { WallWidthThickness * 0.5f, HalfHeight }, { WallWidthThickness, Height });
	transformRegistry.Add(BoundaryTopHandle, { HalfWidth, Height - WallHeightThickness * 0.5f }, { Width - 2.0f * WallWidthThickness, WallHeightThickness });
	transformRegistry.Add(BoundaryRightHandle, { Width - WallWidthThickness * 0.5f, HalfHeight }, { WallWidthThickness, Height });
	transformRegistry.Add(BoundaryBottomHandle, { HalfWidth, WallHeightThickness * 0.5f }, { Width - 2.0f * WallWidthThickness, WallHeightThickness });
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

		Timer.Step();

		if (State == Playing)
		{
			auto* player1Ptr = transformRegistry.TryGetByOwnerMutable(Player1Handle);
			auto* player2Ptr = transformRegistry.TryGetByOwnerMutable(Player2Handle);
			auto* ballPtr = transformRegistry.TryGetByOwnerMutable(BallHandle);

			if (!(player1Ptr && player2Ptr && ballPtr))
			{
				Ludus::Engine::Utilities::WriteLine("[Movement Integration] Missing transform(s).");
				continue;
			}

			auto& player1Transform = *player1Ptr;
			auto& player2Transform = *player2Ptr;
			auto& ballTransform = *ballPtr;

			// Player 1.
			const auto player1Direction = (window.GetInput().GetKey(Key::W) ? 1.0f : 0.0f) + (window.GetInput().GetKey(Key::S) ? -1.0f : 0.0f);
			player1Transform.Position.Y += player1Direction * Player1Speed * Timer;

			// Player 2.
			auto player2Direction = 0.0f;
			if (IsMultiplayer)
			{
				player2Direction = (window.GetInput().GetKey(Key::Up) ? 1.0f : 0.0f) + (window.GetInput().GetKey(Key::Down) ? -1.0f : 0.0f);
			}
			else
			{
				// Player 2 AI.
				if (IsRunning && fabs(ballTransform.Position.Y - player2Transform.Position.Y) > BallSize && ballTransform.Position.X >= HalfWidth)
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
			if (IsRunning)
			{
				const auto ballVelocity = ballTransform.Forward() * (IsBallServed ? BallSpeed : BallSpeed * 0.5f);
				ballTransform.Position += ballVelocity * Timer;
			}
		}

#pragma endregion

#pragma region Collision Handling

		if (State == Playing)
		{
			collisionManager.Step(colliderRegistry, transformRegistry);

			const auto maskBall = LayerMask::NameToLayer(BallLayerName);
			const auto maskBoundaryHorizontal = LayerMask::NameToLayer(BoundaryHorizontalLayerName);
			const auto maskBoundaryVertical = LayerMask::NameToLayer(BoundaryVerticalLayerName);
			const auto maskPlayer1 = LayerMask::NameToLayer(Player1LayerName);
			const auto maskPlayer2 = LayerMask::NameToLayer(Player2LayerName);

			const auto& collisionInfo = collisionManager.GetCollisionInfo();
			for (const auto& info : collisionInfo)
			{
				const auto ownerHandleA = info.CollisionAOwnerHandle;
				const auto ownerHandleB = info.CollisionBOwnerHandle;

				const auto& contactPoint = info.Point;

				const auto* colliderAPtr = colliderRegistry.TryGetByOwnerMutable(ownerHandleA);
				const auto* colliderBPtr = colliderRegistry.TryGetByOwnerMutable(ownerHandleB);

				if (!(colliderAPtr && colliderBPtr))
				{
					Ludus::Engine::Utilities::WriteLine("[Collision Handling] Missing transform(s).");
					continue;
				}

				const auto& colliderA = *colliderAPtr;
				const auto& colliderB = *colliderBPtr;

				auto* transformAPtr = transformRegistry.TryGetByOwnerMutable(ownerHandleA);
				auto* transformBPtr = transformRegistry.TryGetByOwnerMutable(ownerHandleB);

				if (!(transformAPtr && transformBPtr))
				{
					Ludus::Engine::Utilities::WriteLine("[Collision Handling] Missing colliders(s).");
					continue;
				}

				auto& transformA = *transformAPtr;
				auto& transformB = *transformBPtr;

				const auto maskA = LayerMask::FromIndex(colliderA.LayerIndex);
				const auto maskB = LayerMask::FromIndex(colliderB.LayerIndex);

				if (!IsPair(maskA, maskB, maskBall, maskBoundaryVertical))
				{
					const auto isAStatic = Is(maskB, maskBall) && (Is(maskA, maskPlayer1) || Is(maskA, maskPlayer2)) ? true : colliderA.IsStatic;
					const auto isBStatic = Is(maskA, maskBall) && (Is(maskB, maskPlayer1) || Is(maskB, maskPlayer2)) ? true : colliderB.IsStatic;
					const auto correction = contactPoint.Normal * contactPoint.Penetration;
					collisionManager.ResolveCollision(transformAPtr, transformBPtr, isAStatic, isBStatic, correction);
				}

				auto* ballTransform =
					(colliderA.LayerIndex == LayerMask::NameToLayerIndex(BallLayerName)) ? &transformA :
					(colliderB.LayerIndex == LayerMask::NameToLayerIndex(BallLayerName)) ? &transformB : nullptr;

				if (ballTransform)
				{
					if (IsPair(maskA, maskB, maskBall, maskBoundaryHorizontal))
					{
						Ludus::Engine::Utilities::WriteLine("[Collision Handling] Ball <-> Boundary (horizontal)");

						const Vector2D normal = (ballTransform->Position.Y > HalfHeight) ? Vector2D(0.0f, -1.0f) : Vector2D(0.0f, 1.0f);
						if (Vector2D::Dot(ballTransform->Forward(), normal) < 0.0f)
						{
							const auto reflectionVector = Vector2D::Reflect(ballTransform->Forward(), normal);
							ballTransform->Rotate(reflectionVector);
							ballTransform->Position += normal * 0.25f; // Add tolerance.
						}
					}
					else if (IsPair(maskA, maskB, maskBall, maskPlayer1))
					{
						Ludus::Engine::Utilities::WriteLine("[Collision Handling] Ball <-> Player 1");

						const auto& player1Transform = transformRegistry.TryGetByOwner(Player1Handle);
						if (!player1Transform)
						{
							Ludus::Engine::Utilities::WriteLine("[Collision Handling] Missing transform(s).");
							continue;
						}

						const Vector2D normal = { 1.0f, 0.0f };
						if (Vector2D::Dot(ballTransform->Forward(), normal) < 0.0f)
						{
							const auto reflectionAngle = GetReflectionAngle(ballTransform, player1Transform, normal);
							ballTransform->Rotation = reflectionAngle;
							ballTransform->Position += normal * 0.25f; // Add tolerance.
						}
					}
					else if (IsPair(maskA, maskB, maskBall, maskPlayer2))
					{
						Ludus::Engine::Utilities::WriteLine("[Collision Handling] Ball <-> Player 2");

						const auto& player2Transform = transformRegistry.TryGetByOwner(Player2Handle);
						if (!player2Transform)
						{
							Ludus::Engine::Utilities::WriteLine("[Collision Handling] Missing transform(s).");
							continue;
						}

						const Vector2D normal = { -1.0f, 0.0f };
						if (Vector2D::Dot(ballTransform->Forward(), normal) < 0.0f)
						{
							const auto reflectionAngle = GetReflectionAngle(ballTransform, player2Transform, normal);
							ballTransform->Rotation = reflectionAngle;
							ballTransform->Position += normal * 0.25f; // Add tolerance.
						}
					}
					else
					{
						Ludus::Engine::Utilities::WriteLine("[Collision Handling] Ball <-> Boundary (vertical)");

						if (ballTransform->Position.X < HalfWidth)
						{
							Player2Score++;
							Clear();
						}
						else if (ballTransform->Position.X > HalfWidth)
						{
							Player1Score++;
							Clear();
						}
					}

					IsBallServed = true;
				}
				else
				{
					if (IsPair(maskA, maskB, maskPlayer1, maskBoundaryHorizontal))
					{
						Ludus::Engine::Utilities::WriteLine("[Collision Handling] Player 1 <-> Boundary (horizontal)");
					}
					else
					{
						Ludus::Engine::Utilities::WriteLine("[Collision Handling] Player 2 <-> Boundary (horizontal)");
					}
				}
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
	}
}
