#include <algorithm> 
#include <vector>

#include <Ludus/Engine/ColliderRegistry.h>
#include <Ludus/Engine/Cooldown.h>
#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Engine/TimeStep.h>
#include <Ludus/Engine/TransformRegistry.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/Renderer2D.h>
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
using Ludus::Graphics::Renderer2D;
using Ludus::Graphics::GLContext;
using Ludus::Math::Transform2D;
using Ludus::Math::Vector2D;
using Ludus::Physics::Collision2DManager;
using Ludus::Physics::Collider2D;
using Ludus::Platform::Key;

namespace Colors = Ludus::Graphics::Colors;
namespace Numeric = Ludus::Math::Numeric;

const int Width = 800;
const int Height = 640;

const float Player1Speed = 200.0f;
const float Player2Speed = 200.0f;

const float PlayerSize = 40.0f;
const float PlayerBarrelWidth = 20.0f;
const float PlayerBarrelHeight = 10.0f;
const float PlayerStartOffset = Width * 0.1f;


const float PlayerRotationSpeed = 200.0f;
const float ProjectileSpeed = 1000.0f;
const float ProjectileSize = 10.0f;
const int MaxProjectileBounces = 5;
const int MaxActiveProjectiles = 1;

const int QuarterWidth = Width / 4;
const float PaddingHeight = Height * 0.1f;
const float WallThickness = 20.0f;
const float WallHeight = Height - PaddingHeight - WallThickness;

const std::string Player1LayerName = "Player1";
const std::string Player2LayerName = "Player2";
const std::string Player1ProjectileLayerName = "Player1Projectile";
const std::string Player2ProjectileLayerName = "Player2Projectile";
const std::string BoundaryLayerName = "Boundary";

GameObjectHandle Player1Handle;
GameObjectHandle Player2Handle;

GameObjectHandle BoundaryLeftHandle;
GameObjectHandle BoundaryTopHandle;
GameObjectHandle BoundaryRightHandle;
GameObjectHandle BoundaryBottomHandle;
GameObjectHandle BoundaryCenterHandle;

std::vector<GameObject> GameObjects;
std::vector<GameObjectHandle> Projectiles;

LayerMask TankOnTankCollision;
LayerMask TankOnBoundaryCollision;
LayerMask ProjectileOnBoundaryCollision;
LayerMask ProjectileOnPlayer1Collision;
LayerMask ProjectileOnPlayer2Collision;

Ludus::Engine::TimeStep Timer;
Ludus::Engine::Cooldown Player1ProjectileFireRate(0.5f);
Ludus::Engine::Cooldown Player2ProjectileFireRate(0.5f);

Ludus::Engine::TransformRegistry transformRegistry;
Ludus::Engine::ColliderRegistry colliderRegistry;
Collision2DManager collisionManager;

int GameWidth = Width;
int GameHeight = (int)WallHeight;

int Player1Score = 0;
int Player2Score = 0;

int Player1ProjectileCount = 0;
int Player2ProjectileCount = 0;
int Player1ProjectileBounceCount = 0;
int Player2ProjectileBounceCount = 0;

bool isMultiplayer = false;


#pragma region State helpers

void static SpawnProjectile(GameObjectHandle handle, std::string layerName, LayerMask layerMask)
{
	auto transform = transformRegistry.TryGetByOwner(handle);
	if (transform)
	{
		GameObject projectileObject;

		colliderRegistry.Add(
			projectileObject.Handle,
			LayerMask::NameToLayerIndex(layerName),
			layerMask
		);
		transformRegistry.Add(
			projectileObject.Handle,
			transform->Position + transform->Forward() * PlayerSize,
			ProjectileSize,
			transform->Rotation
		);
		Projectiles.push_back(projectileObject.Handle);
		GameObjects.push_back(projectileObject);
	}
}

#pragma endregion

#pragma region Rendering helpers

void static DrawBackground(Renderer2D& renderer)
{
	// Draw the scoreboard. If single player, the other player's score is not shown.
	auto player1ScoreString = std::to_string(Player1Score);
	renderer.DrawText(Transform2D(0, Vector2D(QuarterWidth, Height - PaddingHeight)), player1ScoreString, Colors::White);

	if (isMultiplayer)
	{
		auto player2ScoreString = std::to_string(Player2Score);
		renderer.DrawText(Transform2D(0, Vector2D(Width - QuarterWidth, Height - PaddingHeight)), player2ScoreString, Colors::White);
	}

	// Draw lines that surrounds the game area (Left, Top, Right, Bottom).
	auto boundaryLeftTransform = transformRegistry.TryGetByOwner(BoundaryLeftHandle);
	if (boundaryLeftTransform)
	{
		renderer.DrawQuad(*boundaryLeftTransform, Colors::White);
	}
	auto boundaryTopTransform = transformRegistry.TryGetByOwner(BoundaryTopHandle);
	if (boundaryTopTransform)
	{
		renderer.DrawQuad(*boundaryTopTransform, Colors::White);
	}
	auto boundaryRightTransform = transformRegistry.TryGetByOwner(BoundaryRightHandle);
	if (boundaryRightTransform)
	{
		renderer.DrawQuad(*boundaryRightTransform, Colors::White);
	}
	auto boundaryBottomTransform = transformRegistry.TryGetByOwner(BoundaryBottomHandle);
	if (boundaryBottomTransform)
	{
		renderer.DrawQuad(*boundaryBottomTransform, Colors::White);
	}

	auto boundaryCenterTransform = transformRegistry.TryGetByOwner(BoundaryCenterHandle);
	if (boundaryCenterTransform)
	{
		renderer.DrawQuad(*boundaryCenterTransform, Colors::White);
	}
}

void static DrawPlayers(Renderer2D& renderer)
{
	auto player1Transform = transformRegistry.TryGetByOwner(Player1Handle);
	if (player1Transform)
	{
		renderer.DrawCircle(*player1Transform, Colors::Red);
		renderer.DrawQuad(Transform2D(player1Transform->OwnerHandle, player1Transform->Position + player1Transform->Forward() * PlayerSize * 0.5f, { PlayerBarrelWidth, PlayerBarrelHeight }, player1Transform->Rotation), Colors::Red);
	}
	auto player2Transform = transformRegistry.TryGetByOwner(Player2Handle);
	if (player2Transform)
	{
		renderer.DrawCircle(*player2Transform, Colors::Blue);
		renderer.DrawQuad(Transform2D(player2Transform->OwnerHandle, player2Transform->Position + player2Transform->Forward() * PlayerSize * 0.5f, { PlayerBarrelWidth, PlayerBarrelHeight }, player2Transform->Rotation), Colors::Blue);
	}
}

void static DrawProjectiles(Renderer2D& renderer)
{
	for (auto& projectileOwnerHandle : Projectiles)
	{
		auto projectileCollider = colliderRegistry.TryGetByOwner(projectileOwnerHandle);
		auto projectileTransform = transformRegistry.TryGetByOwner(projectileOwnerHandle);

		if (projectileCollider && projectileTransform)
		{
			auto color = LayerMask::LayerIndexToName(projectileCollider->LayerIndex) == Player1ProjectileLayerName ? Colors::Orange : Colors::Cyan;
			renderer.DrawCircle(*projectileTransform, color);
		}
	}
}

#pragma endregion


int main()
{

#pragma region Initialization

	auto windowOptions = Ludus::Platform::WindowOptions(Width, Height, "Combat (1977)", false);
	auto window = Ludus::Platform::Window(windowOptions);

	GLContext::Init();
	GLContext::EnableBlending();
	GLContext::SetBlendAlpha();

	Ludus::Graphics::Shader shader("Resources/Shaders");
	Ludus::Graphics::Camera2D camera;
	camera.SetViewport(Width, Height);
	camera.SetPosition({ 0.0f, 0.0f });
	camera.SetZoom(1.0f);
	camera.SetRotation(0.0f);

	Renderer2D renderer(shader);

	renderer.SetClearColor(Colors::Black);

#pragma endregion

#pragma region Game objects setup

	// Game objects.
	GameObject player1Object;
	GameObject player2Object;

	GameObject boundaryLeftObject;
	GameObject boundaryTopObject;
	GameObject boundaryRightObject;
	GameObject boundaryBottomObject;
	GameObject boundaryCenterObject;

	Player1Handle = player1Object.Handle;
	Player2Handle = player2Object.Handle;

	BoundaryLeftHandle = boundaryLeftObject.Handle;
	BoundaryTopHandle = boundaryTopObject.Handle;
	BoundaryRightHandle = boundaryRightObject.Handle;
	BoundaryBottomHandle = boundaryBottomObject.Handle;
	BoundaryCenterHandle = boundaryCenterObject.Handle;

	// Layer masks.
	LayerMask::AddLayer(Player1LayerName, 1);
	LayerMask::AddLayer(Player2LayerName, 2);
	LayerMask::AddLayer(Player1ProjectileLayerName, 3);
	LayerMask::AddLayer(Player2ProjectileLayerName, 4);
	LayerMask::AddLayer(BoundaryLayerName, 5);

	// Colliders
	colliderRegistry.Add(Player1Handle, LayerMask::NameToLayerIndex(Player1LayerName), LayerMask::GetMask({ Player2LayerName, Player2ProjectileLayerName, BoundaryLayerName }));
	colliderRegistry.Add(Player2Handle, LayerMask::NameToLayerIndex(Player2LayerName), LayerMask::GetMask({ Player1LayerName, Player1ProjectileLayerName, BoundaryLayerName }));

	auto boundaryCollisionMask = LayerMask::GetMask({ Player1LayerName, Player2LayerName, Player1ProjectileLayerName, Player2ProjectileLayerName });
	colliderRegistry.Add(BoundaryLeftHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderRegistry.Add(BoundaryTopHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderRegistry.Add(BoundaryRightHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderRegistry.Add(BoundaryBottomHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderRegistry.Add(BoundaryCenterHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);

	// Transforms.
	transformRegistry.Add(Player1Handle, { PlayerStartOffset, WallHeight * 0.5f }, { PlayerSize }, 0.0f);
	transformRegistry.Add(Player2Handle, { Width - PlayerStartOffset, WallHeight * 0.5f }, { PlayerSize }, 180.0f);

	transformRegistry.Add(BoundaryLeftHandle, { WallThickness * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight }, 0.0f);
	transformRegistry.Add(BoundaryTopHandle, { Width * 0.5f, WallHeight - WallThickness * 0.5f }, { Width, WallThickness }, 0.0f);
	transformRegistry.Add(BoundaryRightHandle, { Width - WallThickness * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight }, 0.0f);
	transformRegistry.Add(BoundaryBottomHandle, { Width * 0.5f, WallThickness * 0.5f }, { Width, WallThickness }, 0.0f);
	transformRegistry.Add(BoundaryCenterHandle, { Width * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight * 0.25 }, 0.0f);

	// Game objects.
	GameObjects.push_back(player1Object);
	GameObjects.push_back(player2Object);

	GameObjects.push_back(boundaryLeftObject);
	GameObjects.push_back(boundaryTopObject);
	GameObjects.push_back(boundaryRightObject);
	GameObjects.push_back(boundaryBottomObject);
	GameObjects.push_back(boundaryCenterObject);

#pragma endregion

	while (!window.WindowShouldClose())
	{

#pragma region Input Handling

		Timer.Step();
		Player1ProjectileFireRate.Step(Timer);
		Player2ProjectileFireRate.Step(Timer);

		// Player inputs.
		int player1Thrust = (window.GetInput().GetKey(Key::W) ? +1 : 0) + (window.GetInput().GetKey(Key::S) ? -1 : 0);
		int player1Turn = (window.GetInput().GetKey(Key::A) ? +1 : 0) + (window.GetInput().GetKey(Key::D) ? -1 : 0);

		// If multiplayer is selected, the second player keyboard inputs are enabled.
		int player2Thrust;
		int player2Turn;

		if (isMultiplayer)
		{
			player2Thrust = (window.GetInput().GetKey(Key::Up) ? +1 : 0) + (window.GetInput().GetKey(Key::Down) ? -1 : 0);
			player2Turn = (window.GetInput().GetKey(Key::Left) ? +1 : 0) + (window.GetInput().GetKey(Key::Right) ? -1 : 0);
		}
		else
		{
			player2Thrust = 0;
			player2Turn = 0;

			// TODO: Create player 2 movement AI.

			// 1. Simple look at player test.
			auto player1Transform = transformRegistry.TryGetByOwner(Player1Handle);
			auto player2Transform = transformRegistry.TryGetByOwnerMutable(Player2Handle);
			if (player1Transform && player2Transform)
			{
				// TODO: Refactor logic into Numeric and Vector2D static methods.
				auto objectPos = player2Transform->Position;
				auto targ = player1Transform->Position;
				targ.X = targ.X - objectPos.X;
				targ.Y = targ.Y - objectPos.Y;

				float angle = Numeric::RadiansToDegrees(std::atan2f(targ.Y, targ.X));
				player2Transform->Rotation = angle;
			}

			// 2. ...
		}

		// Projectile inputs.
		if (window.GetInput().GetKeyDown(Key::Space) && Player1ProjectileFireRate.CanFire() && Player1ProjectileCount < MaxActiveProjectiles)
		{
			SpawnProjectile(Player1Handle, Player1ProjectileLayerName, LayerMask::GetMask({ Player2LayerName, BoundaryLayerName }));
			Player1ProjectileFireRate.Consume();

			Player1ProjectileCount++;
		}

		if (isMultiplayer)
		{
			if (window.GetInput().GetKeyDown(Key::Enter) && Player2ProjectileFireRate.CanFire() && Player2ProjectileCount < MaxActiveProjectiles)
			{
				SpawnProjectile(Player2Handle, Player2ProjectileLayerName, LayerMask::GetMask({ Player1LayerName, BoundaryLayerName }));
				Player2ProjectileFireRate.Consume();

				Player2ProjectileCount++;
			}
		}
		else
		{
			// TODO: Create player 2 attack AI.

			// 1. Simple attack when within range.
			if (Player2ProjectileFireRate.CanFire() && Player2ProjectileCount < MaxActiveProjectiles)
			{
				auto player1Transform = transformRegistry.TryGetByOwner(Player1Handle);
				auto player2Transform = transformRegistry.TryGetByOwnerMutable(Player2Handle);

				if (player1Transform && player2Transform && std::abs(player1Transform->Position.X - player2Transform->Position.X) < GameWidth * 0.5f)
				{
					SpawnProjectile(Player2Handle, Player2ProjectileLayerName, LayerMask::GetMask({ Player1LayerName, BoundaryLayerName }));
					Player2ProjectileFireRate.Consume();

					Player2ProjectileCount++;
				}
			}

			// 2. ...
		}

		if (window.GetInput().GetKeyDown(Key::Escape))
		{
			window.SetWindowShouldClose();
		}

#pragma endregion

#pragma region Movement Integration

		// Player movement.
		auto player1Transform = transformRegistry.TryGetByOwnerMutable(Player1Handle);
		if (player1Transform)
		{
			player1Transform->Rotation += player1Turn * PlayerRotationSpeed * Timer;
			player1Transform->Position += player1Transform->Forward() * (player1Thrust * Player1Speed * Timer);
		}

		auto player2Transform = transformRegistry.TryGetByOwnerMutable(Player2Handle);
		if (player2Transform)
		{
			player2Transform->Rotation += player2Turn * PlayerRotationSpeed * Timer;
			player2Transform->Position += player2Transform->Forward() * (player2Thrust * Player2Speed * Timer);
		}

		// Projectile movement.
		for (auto& projectileOwnerHandle : Projectiles)
		{
			auto projectileTransform = transformRegistry.TryGetByOwnerMutable(projectileOwnerHandle);
			if (projectileTransform)
			{
				auto velocity = projectileTransform->Forward() * ProjectileSpeed;
				projectileTransform->Position += velocity * Timer;
			}
		}

#pragma endregion

#pragma region Collision Handling

		collisionManager.Step(colliderRegistry, transformRegistry);

		// TODO: Create combinations as const(expr) in the start of the program.
		auto maskPlayer1 = LayerMask::NameToLayer(Player1LayerName);
		auto maskPlayer2 = LayerMask::NameToLayer(Player2LayerName);
		auto maskPlayer1Projectile = LayerMask::NameToLayer(Player1ProjectileLayerName);
		auto maskPlayer2Projectile = LayerMask::NameToLayer(Player2ProjectileLayerName);
		auto maskBoundary = LayerMask::NameToLayer(BoundaryLayerName);

		for (auto& collision : collisionManager.GetCollisionInfo())
		{
			auto ownerHandleA = collision.CollisionAOwnerHandle;
			auto ownerHandleB = collision.CollisionBOwnerHandle;

			auto colliderA = colliderRegistry.TryGetByOwner(ownerHandleA);
			auto colliderB = colliderRegistry.TryGetByOwner(ownerHandleB);

			if (!colliderA || !colliderB)
			{
				continue;
			}

			auto maskA = LayerMask::FromIndex(colliderA->LayerIndex);
			auto maskB = LayerMask::FromIndex(colliderB->LayerIndex);

			auto transformA = transformRegistry.TryGetByOwnerMutable(ownerHandleA);
			auto transformB = transformRegistry.TryGetByOwnerMutable(ownerHandleB);

			Vector2D correction = collision.Point.Normal * collision.Point.Penetration;

			collisionManager.ResolveCollision(transformA, transformB, colliderA->IsStatic, colliderB->IsStatic, correction);

			// TODO: Use the LayerMask as the value directly, and perform fast bit operations using a switch statement.
			if ((maskA == maskPlayer2 && maskB == maskPlayer1Projectile) || (maskA == maskPlayer1Projectile && maskB == maskPlayer2))
			{
				std::cout << "Player 2 hit by a Player 1's projectile!" << std::endl;

				colliderRegistry.RemoveByOwner(ownerHandleA);
				colliderRegistry.RemoveByOwner(ownerHandleB);

				transformRegistry.RemoveByOwner(ownerHandleA);
				transformRegistry.RemoveByOwner(ownerHandleB);

				Player1ProjectileCount = 0;
				Player1ProjectileBounceCount = 0;
			}

			if ((maskA == maskPlayer1 && maskB == maskPlayer2Projectile) || (maskA == maskPlayer2Projectile && maskB == maskPlayer1))
			{
				std::cout << "Player 1 hit by Player 2's projectile!" << std::endl;

				colliderRegistry.RemoveByOwner(ownerHandleA);
				colliderRegistry.RemoveByOwner(ownerHandleB);

				transformRegistry.RemoveByOwner(ownerHandleA);
				transformRegistry.RemoveByOwner(ownerHandleB);

				Player2ProjectileCount = 0;
				Player2ProjectileBounceCount = 0;
			}

			if ((maskA == maskBoundary && maskB == maskPlayer1Projectile) || (maskA == maskPlayer1Projectile && maskB == maskBoundary))
			{
				std::cout << "Player 1's projectile collided with a boundary!" << std::endl;

				auto ownerHandle = maskA == maskPlayer1Projectile ? ownerHandleA : ownerHandleB;
				auto transform = transformRegistry.TryGetByOwnerMutable(ownerHandle);
				if (!transform)
				{
					break;
				}

				if (Player1ProjectileBounceCount < MaxProjectileBounces - 1)
				{
					auto normal = collision.Point.Normal;
					auto direction = transform->Forward();

					direction = Vector2D::Reflect(direction, normal).Normalize();
					transform->Rotation = Numeric::RadiansToDegrees(std::atan2(direction.Y, direction.X));

					Player1ProjectileBounceCount++;
				}
				else
				{
					colliderRegistry.RemoveByOwner(ownerHandle);
					transformRegistry.RemoveByOwner(ownerHandle);

					Player1ProjectileBounceCount = 0;
					Player1ProjectileCount = 0;
				}
			}

			if ((maskA == maskBoundary && maskB == maskPlayer2Projectile) || (maskA == maskPlayer2Projectile && maskB == maskBoundary))
			{
				std::cout << "Player 2's projectile collided with a boundary!" << std::endl;

				auto ownerHandle = maskA == maskPlayer2Projectile ? ownerHandleA : ownerHandleB;
				auto transform = transformRegistry.TryGetByOwnerMutable(ownerHandle);
				if (!transform)
				{
					break;
				}

				if (Player2ProjectileBounceCount < MaxProjectileBounces - 1)
				{
					auto normal = collision.Point.Normal;
					auto direction = transform->Forward();

					direction = Vector2D::Reflect(direction, normal).Normalize();
					transform->Rotation = Numeric::RadiansToDegrees(std::atan2(direction.Y, direction.X));

					Player2ProjectileBounceCount++;
				}
				else
				{
					colliderRegistry.RemoveByOwner(ownerHandle);
					transformRegistry.RemoveByOwner(ownerHandle);

					Player2ProjectileBounceCount = 0;
					Player2ProjectileCount = 0;
				}
			}

			if ((maskA == maskPlayer2 && maskB == maskBoundary) || (maskA == maskBoundary && maskB == maskPlayer2))
			{
				std::cout << "Player 2 collided with a boundary!" << std::endl;
			}

			if ((maskA == maskPlayer1 && maskB == maskBoundary) || (maskA == maskBoundary && maskB == maskPlayer1))
			{
				std::cout << "Player 1 collided with a boundary!" << std::endl;
			}

			if ((maskA == maskPlayer1 && maskB == maskPlayer2) || (maskA == maskPlayer2 && maskB == maskPlayer1))
			{
				std::cout << "Player 1 and Player 2 Collided!" << std::endl;
			}
		}

#pragma endregion

#pragma region Rendering

		renderer.BeginScene(camera);
		renderer.Clear();

		DrawBackground(renderer);
		DrawPlayers(renderer);
		DrawProjectiles(renderer);

		renderer.EndScene();

#pragma endregion

		window.SwapBuffers();
		window.PollEvents();
	}
}
