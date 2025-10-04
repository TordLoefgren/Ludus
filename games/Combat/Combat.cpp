#include <algorithm> 
#include <core/Cooldown.h>
#include <core/TimeStep.h>
#include <graphics/Camera2D.h>
#include <graphics/GameObject.h>
#include <graphics/GLContext.h>
#include <graphics/Renderer2D.h>
#include <graphics/Transform2D.h>
#include <math/Numeric.h>
#include <math/Vector2D.h>
#include <optional>
#include <physics/Collider2D.h>
#include <physics/ColliderSystem.h>
#include <physics/Collision2DManager.h>
#include <physics/LayerMask.h>
#include <physics/TransformSystem.h>
#include <vector>
#include <windowing/Window.h>

using GameObjectHandle = uint32_t;

const int Width = 800;
const int Height = 640;

std::vector<GameObject> GameObjects;

GameObjectHandle Player1Handle;
GameObjectHandle Player2Handle;

GameObjectHandle BoundaryLeftHandle;
GameObjectHandle BoundaryTopHandle;
GameObjectHandle BoundaryRightHandle;
GameObjectHandle BoundaryBottomHandle;
GameObjectHandle BoundaryCenterHandle;

const std::string PlayerLayerName = "Player";
const std::string EnemyLayerName = "Enemy";
const std::string PlayerProjectileLayerName = "PlayerProjectile";
const std::string EnemyProjectileLayerName = "EnemyProjectile";
const std::string BoundaryLayerName = "Boundary";

LayerMask TankOnTankCollision;
LayerMask TankOnBoundaryCollision;
LayerMask ProjectileOnBoundaryCollision;
LayerMask ProjectileOnPlayerCollision;
LayerMask ProjectileOnEnemyCollision;

TimeStep Timer;
Cooldown PlayerProjectileFireRate(0.5f);
Cooldown EnemyProjectileFireRate(0.5f);

TransformSystem transformSystem;
ColliderSystem colliderSystem;
Collision2DManager collisionManager;

const float Player1Speed = 200.0f;
const float Player2Speed = 200.0f;

const float PlayerSize = 40.0f;
const float PlayerBarrelWidth = 20.0f;
const float PlayerBarrelHeight = 10.0f;
const float PlayerStartOffset = Width * 0.1f;

std::vector<GameObjectHandle> Projectiles;

const float PlayerRotationSpeed = 200.0f;
const float ProjectileSpeed = 400.0f;
const float ProjectileSize = 10.0f;

const int QuarterWidth = Width / 4;
const float PaddingHeight = Height * 0.1f;
const float WallThickness = 20.0f;
const float WallHeight = Height - PaddingHeight - WallThickness;

int GameWidth = Width;
int GameHeight = (int)WallHeight;

int Player1Score = 0;
int Player2Score = 0;

bool isMultiplayer = false;

void SpawnProjectile(GameObjectHandle handle, std::string layerName, LayerMask layerMask)
{
	auto transform = transformSystem.TryGetByOwner(handle);
	if (transform)
	{
		GameObject projectileObject;

		colliderSystem.Add(
			projectileObject.Handle,
			LayerMask::NameToLayerIndex(layerName),
			layerMask
		);
		transformSystem.Add(
			projectileObject.Handle,
			transform->Position + transform->Forward() * PlayerSize,
			ProjectileSize,
			transform->Rotation
		);
		Projectiles.push_back(projectileObject.Handle);
		GameObjects.push_back(projectileObject);
	}
}

#pragma region Rendering helpers

void static DrawBackground(Renderer2D& renderer)
{
	// Draw the scoreboard. If single player, the enemy score is not shown.
	auto player1ScoreString = std::to_string(Player1Score);
	renderer.DrawText(Transform2D(0, Vector2D(QuarterWidth, Height - PaddingHeight)), Colors::White, player1ScoreString);

	if (isMultiplayer)
	{
		auto player2ScoreString = std::to_string(Player2Score);
		renderer.DrawText(Transform2D(0, Vector2D(Width - QuarterWidth, Height - PaddingHeight)), Colors::White, player2ScoreString);
	}

	// Draw lines that surrounds the game area (Left, Top, Right, Bottom).

	auto boundaryLeftTransform = transformSystem.TryGetByOwner(BoundaryLeftHandle);
	if (boundaryLeftTransform)
	{
		renderer.DrawQuad(*boundaryLeftTransform, Colors::White);
	}
	auto boundaryTopTransform = transformSystem.TryGetByOwner(BoundaryTopHandle);
	if (boundaryTopTransform)
	{
		renderer.DrawQuad(*boundaryTopTransform, Colors::White);
	}
	auto boundaryRightTransform = transformSystem.TryGetByOwner(BoundaryRightHandle);
	if (boundaryRightTransform)
	{
		renderer.DrawQuad(*boundaryRightTransform, Colors::White);
	}
	auto boundaryBottomTransform = transformSystem.TryGetByOwner(BoundaryBottomHandle);
	if (boundaryBottomTransform)
	{
		renderer.DrawQuad(*boundaryBottomTransform, Colors::White);
	}

	auto boundaryCenterTransform = transformSystem.TryGetByOwner(BoundaryCenterHandle);
	if (boundaryCenterTransform)
	{
		renderer.DrawQuad(*boundaryCenterTransform, Colors::White);
	}
}

void static DrawPlayers(Renderer2D& renderer)
{
	auto player1Transform = transformSystem.TryGetByOwner(Player1Handle);
	if (player1Transform)
	{
		renderer.DrawCircle(*player1Transform, Colors::Red);
		renderer.DrawQuad(Transform2D(player1Transform->OwnerHandle, player1Transform->Position + player1Transform->Forward() * PlayerSize * 0.5f, { PlayerBarrelWidth, PlayerBarrelHeight }, player1Transform->Rotation), Colors::Red);
	}
	auto player2Transform = transformSystem.TryGetByOwner(Player2Handle);
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
		auto projectileCollider = colliderSystem.TryGetByOwner(projectileOwnerHandle);
		auto projectileTransform = transformSystem.TryGetByOwner(projectileOwnerHandle);

		if (projectileCollider && projectileTransform)
		{
			auto color = LayerMask::LayerIndexToName(projectileCollider->LayerIndex) == PlayerProjectileLayerName ? Colors::Orange : Colors::Cyan;
			renderer.DrawCircle(*projectileTransform, color);
		}
	}
}

#pragma endregion


int main()
{

#pragma region Startup

	auto windowOptions = WindowOptions(Width, Height, "Combat (1977)", false);
	auto window = Window(windowOptions);

	GLContext::Init();
	GLContext::EnableBlending();
	GLContext::SetBlendAlpha();

	Shader shader("resources/shaders");
	Camera2D camera;
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
	LayerMask::AddLayer(PlayerLayerName, 1);
	LayerMask::AddLayer(EnemyLayerName, 2);
	LayerMask::AddLayer(PlayerProjectileLayerName, 3);
	LayerMask::AddLayer(EnemyProjectileLayerName, 4);
	LayerMask::AddLayer(BoundaryLayerName, 5);

	// Colliders
	colliderSystem.Add(Player1Handle, LayerMask::NameToLayerIndex(PlayerLayerName), LayerMask::GetMask({ EnemyLayerName, EnemyProjectileLayerName, BoundaryLayerName }));
	colliderSystem.Add(Player2Handle, LayerMask::NameToLayerIndex(EnemyLayerName), LayerMask::GetMask({ PlayerLayerName, PlayerProjectileLayerName, BoundaryLayerName }));

	auto boundaryCollisionMask = LayerMask::GetMask({ PlayerLayerName, EnemyLayerName, PlayerProjectileLayerName, EnemyProjectileLayerName });
	colliderSystem.Add(BoundaryLeftHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderSystem.Add(BoundaryTopHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderSystem.Add(BoundaryRightHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderSystem.Add(BoundaryBottomHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	colliderSystem.Add(BoundaryCenterHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);

	// Transforms.
	transformSystem.Add(Player1Handle, { PlayerStartOffset, WallHeight * 0.5f }, { PlayerSize }, 0.0f);
	transformSystem.Add(Player2Handle, { Width - PlayerStartOffset, WallHeight * 0.5f }, { PlayerSize }, 180.0f);

	transformSystem.Add(BoundaryLeftHandle, { WallThickness * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight }, 0.0f);
	transformSystem.Add(BoundaryTopHandle, { Width * 0.5f, WallHeight - WallThickness * 0.5f }, { Width, WallThickness }, 0.0f);
	transformSystem.Add(BoundaryRightHandle, { Width - WallThickness * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight }, 0.0f);
	transformSystem.Add(BoundaryBottomHandle, { Width * 0.5f, WallThickness * 0.5f }, { Width, WallThickness }, 0.0f);
	transformSystem.Add(BoundaryCenterHandle, { Width * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight * 0.25 }, 0.0f);

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
		PlayerProjectileFireRate.Step(Timer);
		EnemyProjectileFireRate.Step(Timer);

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

			// TODO: Create enemy movement AI.

			// 1. Simple look at player test.
			auto player1Transform = transformSystem.TryGetByOwner(Player1Handle);
			auto player2Transform = transformSystem.TryGetByOwnerMutable(Player2Handle);
			if (player1Transform && player2Transform)
			{
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
		if (window.GetInput().GetKeyDown(Key::Space) && PlayerProjectileFireRate.CanFire())
		{
			SpawnProjectile(Player1Handle, PlayerProjectileLayerName, LayerMask::GetMask({ EnemyLayerName, BoundaryLayerName }));
			PlayerProjectileFireRate.Consume();
		}

		if (isMultiplayer)
		{
			if (window.GetInput().GetKeyDown(Key::Enter) && EnemyProjectileFireRate.CanFire())
			{
				SpawnProjectile(Player2Handle, EnemyProjectileLayerName, LayerMask::GetMask({ PlayerLayerName, BoundaryLayerName }));
				EnemyProjectileFireRate.Consume();
			}
		}
		else
		{
			// TODO: Create enemy attack AI.

			// 1. Simple attack when within range.
			if (EnemyProjectileFireRate.CanFire())
			{
				auto player1Transform = transformSystem.TryGetByOwner(Player1Handle);
				auto player2Transform = transformSystem.TryGetByOwnerMutable(Player2Handle);

				if (player1Transform && player2Transform && std::abs(player1Transform->Position.X - player2Transform->Position.X) < GameWidth * 0.5f)
				{
					SpawnProjectile(Player2Handle, EnemyProjectileLayerName, LayerMask::GetMask({ PlayerLayerName, BoundaryLayerName }));
					EnemyProjectileFireRate.Consume();
				}
			}

			// 2. ...
		}

		if (window.GetInput().GetKeyDown(Key::Escape))
		{
			window.SetWindowShouldClose();
		}

#pragma endregion

#pragma region Simulation

		// Player movement.
		auto player1Transform = transformSystem.TryGetByOwnerMutable(Player1Handle);
		if (player1Transform)
		{
			player1Transform->Rotation += player1Turn * PlayerRotationSpeed * Timer;
			player1Transform->Position += player1Transform->Forward() * (player1Thrust * Player1Speed * Timer);
		}

		auto player2Transform = transformSystem.TryGetByOwnerMutable(Player2Handle);
		if (player2Transform)
		{
			player2Transform->Rotation += player2Turn * PlayerRotationSpeed * Timer;
			player2Transform->Position += player2Transform->Forward() * (player2Thrust * Player2Speed * Timer);
		}

		// Projectile movement.
		for (auto& projectileOwnerHandle : Projectiles)
		{
			auto projectileTransform = transformSystem.TryGetByOwnerMutable(projectileOwnerHandle);
			if (projectileTransform)
			{
				auto velocity = projectileTransform->Forward() * ProjectileSpeed;
				projectileTransform->Position += velocity * Timer;
			}
		}

#pragma endregion

#pragma region Collision Handling

		collisionManager.Step(colliderSystem, transformSystem);

		// TODO: Create combinations as const in the start of the program.
		auto maskPlayer = LayerMask::NameToLayer(PlayerLayerName);
		auto maskEnemy = LayerMask::NameToLayer(EnemyLayerName);
		auto maskPlayerProjectile = LayerMask::NameToLayer(PlayerProjectileLayerName);
		auto maskEnemyProjectile = LayerMask::NameToLayer(EnemyProjectileLayerName);
		auto maskBoundary = LayerMask::NameToLayer(BoundaryLayerName);

		for (auto& collision : collisionManager.GetCollisionInfo())
		{
			auto ownerHandleA = collision.CollisionAOwnerHandle;
			auto ownerHandleB = collision.CollisionBOwnerHandle;

			auto colliderA = colliderSystem.TryGetByOwner(ownerHandleA);
			auto colliderB = colliderSystem.TryGetByOwner(ownerHandleB);

			if (!colliderA || !colliderB)
			{
				continue;
			}

			auto maskA = LayerMask::FromIndex(colliderA->LayerIndex);
			auto maskB = LayerMask::FromIndex(colliderB->LayerIndex);

			auto transformA = transformSystem.TryGetByOwnerMutable(ownerHandleA);
			auto transformB = transformSystem.TryGetByOwnerMutable(ownerHandleB);

			Vector2D correction = collision.Point.Normal * collision.Point.Penetration;

			collisionManager.ResolveCollision(transformA, transformB, colliderA->IsStatic, colliderB->IsStatic, correction);

			// TODO: Use the LayerMask as the value directly, and perform fast bit operations using a switch statement.
			if ((maskA == maskEnemy && maskB == maskPlayerProjectile) || (maskA == maskPlayerProjectile && maskB == maskEnemy))
			{
				std::cout << "Enemy hit by a Player projectile!" << std::endl;

				colliderSystem.RemoveByOwner(ownerHandleA);
				colliderSystem.RemoveByOwner(ownerHandleB);

				transformSystem.RemoveByOwner(ownerHandleA);
				transformSystem.RemoveByOwner(ownerHandleB);
			}

			if ((maskA == maskPlayer && maskB == maskEnemyProjectile) || (maskA == maskEnemyProjectile && maskB == maskPlayer))
			{
				std::cout << "Player hit by Enemy projectile!" << std::endl;

				colliderSystem.RemoveByOwner(ownerHandleA);
				colliderSystem.RemoveByOwner(ownerHandleB);

				transformSystem.RemoveByOwner(ownerHandleA);
				transformSystem.RemoveByOwner(ownerHandleB);
			}

			if ((maskA == maskBoundary && maskB == maskPlayerProjectile) || (maskA == maskPlayerProjectile && maskB == maskBoundary))
			{
				std::cout << "Player projectile collided with a boundary!" << std::endl;

				if (maskA == maskPlayerProjectile)
				{
					colliderSystem.RemoveByOwner(ownerHandleA);
					transformSystem.RemoveByOwner(ownerHandleA);
				}
				else
				{
					colliderSystem.RemoveByOwner(ownerHandleB);
					transformSystem.RemoveByOwner(ownerHandleB);
				}
			}

			if ((maskA == maskBoundary && maskB == maskEnemyProjectile) || (maskA == maskEnemyProjectile && maskB == maskBoundary))
			{
				std::cout << "Enemy projectile collided with a boundary!" << std::endl;

				if (maskA == maskEnemyProjectile)
				{
					colliderSystem.RemoveByOwner(ownerHandleA);
					transformSystem.RemoveByOwner(ownerHandleA);
				}
				else
				{
					colliderSystem.RemoveByOwner(ownerHandleB);
					transformSystem.RemoveByOwner(ownerHandleB);
				}
			}

			if ((maskA == maskEnemy && maskB == maskBoundary) || (maskA == maskBoundary && maskB == maskEnemy))
			{
				std::cout << "Enemy collided with a boundary!" << std::endl;
			}

			if ((maskA == maskPlayer && maskB == maskBoundary) || (maskA == maskBoundary && maskB == maskPlayer))
			{
				std::cout << "Player collided with a boundary!" << std::endl;
			}

			if ((maskA == maskPlayer && maskB == maskEnemy) || (maskA == maskEnemy && maskB == maskPlayer))
			{
				std::cout << "Player and Enemy Collided!" << std::endl;
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
