#include <algorithm> 
#include <vector>

#include <Ludus/Engine/ColliderRegistry.h>
#include <Ludus/Engine/Cooldown.h>
#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Engine/Scene.h>
#include <Ludus/Engine/TimeStep.h>
#include <Ludus/Engine/TransformRegistry.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Math/Numeric.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Collider2D.h>
#include <Ludus/Physics/CollisionSystem2D.h>
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
using Ludus::Physics::CollisionSystem2D;
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

Ludus::Engine::Scene scene;

GameObjectHandle Player1Handle;
GameObjectHandle Player2Handle;
GameObjectHandle BoundaryLeftHandle;
GameObjectHandle BoundaryTopHandle;
GameObjectHandle BoundaryRightHandle;
GameObjectHandle BoundaryBottomHandle;
GameObjectHandle BoundaryCenterHandle;

std::vector<GameObjectHandle> Projectiles;

Ludus::Engine::TimeStep Timer;
Ludus::Engine::Cooldown Player1ProjectileFireRate(0.5f);
Ludus::Engine::Cooldown Player2ProjectileFireRate(0.5f);

CollisionSystem2D collisionSystem;

int GameWidth = Width;
int GameHeight = (int)WallHeight;

int Player1Score = 0;
int Player2Score = 0;

int Player1ProjectileCount = 0;
int Player2ProjectileCount = 0;
int Player1ProjectileBounceCount = 0;
int Player2ProjectileBounceCount = 0;

bool IsMultiplayer = false;

#pragma region State helpers

void static SpawnProjectile(GameObjectHandle handle, std::string layerName, LayerMask layerMask)
{
	auto transform = scene.Transforms.TryGetByOwner(handle);
	if (transform)
	{
		auto projectileHandle = scene.AddGameObject();

		scene.AttachCollider(
			projectileHandle,
			LayerMask::NameToLayerIndex(layerName),
			layerMask
		);
		scene.AttachTransform(
			projectileHandle,
			transform->Position + transform->Forward() * PlayerSize,
			ProjectileSize,
			transform->Rotation
		);

		Projectiles.emplace_back(projectileHandle);
	}
}

#pragma endregion

#pragma region Rendering helpers

void static DrawBackground(Renderer2D& renderer)
{
	// Draw the scoreboard. If single player, the other player's score is not shown.
	auto player1ScoreString = std::to_string(Player1Score);
	renderer.DrawText(Transform2D(0, Vector2D(QuarterWidth, Height - PaddingHeight)), player1ScoreString, Colors::White);

	if (IsMultiplayer)
	{
		auto player2ScoreString = std::to_string(Player2Score);
		renderer.DrawText(Transform2D(0, Vector2D(Width - QuarterWidth, Height - PaddingHeight)), player2ScoreString, Colors::White);
	}

	// Draw lines that surrounds the game area (Left, Top, Right, Bottom).
	auto boundaryLeftTransform = scene.Transforms.TryGetByOwner(BoundaryLeftHandle);
	if (boundaryLeftTransform)
	{
		renderer.DrawQuad(*boundaryLeftTransform, Colors::White);
	}
	auto boundaryTopTransform = scene.Transforms.TryGetByOwner(BoundaryTopHandle);
	if (boundaryTopTransform)
	{
		renderer.DrawQuad(*boundaryTopTransform, Colors::White);
	}
	auto boundaryRightTransform = scene.Transforms.TryGetByOwner(BoundaryRightHandle);
	if (boundaryRightTransform)
	{
		renderer.DrawQuad(*boundaryRightTransform, Colors::White);
	}
	auto boundaryBottomTransform = scene.Transforms.TryGetByOwner(BoundaryBottomHandle);
	if (boundaryBottomTransform)
	{
		renderer.DrawQuad(*boundaryBottomTransform, Colors::White);
	}

	auto boundaryCenterTransform = scene.Transforms.TryGetByOwner(BoundaryCenterHandle);
	if (boundaryCenterTransform)
	{
		renderer.DrawQuad(*boundaryCenterTransform, Colors::White);
	}
}

void static DrawPlayers(Renderer2D& renderer)
{
	auto player1Transform = scene.Transforms.TryGetByOwner(Player1Handle);
	if (player1Transform)
	{
		renderer.DrawCircle(*player1Transform, Colors::Red);
		renderer.DrawQuad(Transform2D(player1Transform->OwnerHandle, player1Transform->Position + player1Transform->Forward() * PlayerSize * 0.5f, { PlayerBarrelWidth, PlayerBarrelHeight }, player1Transform->Rotation), Colors::Red);
	}
	auto player2Transform = scene.Transforms.TryGetByOwner(Player2Handle);
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
		auto projectileCollider = scene.Colliders.TryGetByOwner(projectileOwnerHandle);
		auto projectileTransform = scene.Transforms.TryGetByOwner(projectileOwnerHandle);

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

#pragma region Game Objects Setup

	// Game objects.
	Player1Handle = scene.AddGameObject();
	Player2Handle = scene.AddGameObject();
	BoundaryLeftHandle = scene.AddGameObject();
	BoundaryTopHandle = scene.AddGameObject();
	BoundaryRightHandle = scene.AddGameObject();
	BoundaryBottomHandle = scene.AddGameObject();
	BoundaryCenterHandle = scene.AddGameObject();

	// Layer masks.
	LayerMask::AddLayer(Player1LayerName, 1);
	LayerMask::AddLayer(Player2LayerName, 2);
	LayerMask::AddLayer(Player1ProjectileLayerName, 3);
	LayerMask::AddLayer(Player2ProjectileLayerName, 4);
	LayerMask::AddLayer(BoundaryLayerName, 5);

	// Colliders
	scene.AttachCollider(Player1Handle, LayerMask::NameToLayerIndex(Player1LayerName), LayerMask::GetMask({ Player2LayerName, Player2ProjectileLayerName, BoundaryLayerName }));
	scene.AttachCollider(Player2Handle, LayerMask::NameToLayerIndex(Player2LayerName), LayerMask::GetMask({ Player1LayerName, Player1ProjectileLayerName, BoundaryLayerName }));

	auto boundaryCollisionMask = LayerMask::GetMask({ Player1LayerName, Player2LayerName, Player1ProjectileLayerName, Player2ProjectileLayerName });
	scene.AttachCollider(BoundaryLeftHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	scene.AttachCollider(BoundaryTopHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	scene.AttachCollider(BoundaryRightHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	scene.AttachCollider(BoundaryBottomHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);
	scene.AttachCollider(BoundaryCenterHandle, LayerMask::NameToLayerIndex(BoundaryLayerName), boundaryCollisionMask, true);

	// Transforms.
	scene.AttachTransform(Player1Handle, { PlayerStartOffset, WallHeight * 0.5f }, { PlayerSize }, 0.0f);
	scene.AttachTransform(Player2Handle, { Width - PlayerStartOffset, WallHeight * 0.5f }, { PlayerSize }, 180.0f);
	scene.AttachTransform(BoundaryLeftHandle, { WallThickness * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight }, 0.0f);
	scene.AttachTransform(BoundaryTopHandle, { Width * 0.5f, WallHeight - WallThickness * 0.5f }, { Width, WallThickness }, 0.0f);
	scene.AttachTransform(BoundaryRightHandle, { Width - WallThickness * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight }, 0.0f);
	scene.AttachTransform(BoundaryBottomHandle, { Width * 0.5f, WallThickness * 0.5f }, { Width, WallThickness }, 0.0f);
	scene.AttachTransform(BoundaryCenterHandle, { Width * 0.5f, WallHeight * 0.5f }, { WallThickness, WallHeight * 0.25 }, 0.0f);

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

		if (IsMultiplayer)
		{
			player2Thrust = (window.GetInput().GetKey(Key::Up) ? +1 : 0) + (window.GetInput().GetKey(Key::Down) ? -1 : 0);
			player2Turn = (window.GetInput().GetKey(Key::Left) ? +1 : 0) + (window.GetInput().GetKey(Key::Right) ? -1 : 0);
		}
		else
		{
			player2Thrust = 0;
			player2Turn = 0;

			// --- Player 2 movement AI ---

			// Look at player.
			auto player1Transform = scene.Transforms.TryGetByOwner(Player1Handle);
			auto player2Transform = scene.Transforms.TryGetByOwnerMutable(Player2Handle);
			if (player1Transform && player2Transform)
			{
				auto objectPos = player2Transform->Position;
				auto targ = player1Transform->Position;
				targ.X = targ.X - objectPos.X;
				targ.Y = targ.Y - objectPos.Y;

				float angle = Numeric::RadiansToDegrees(std::atan2f(targ.Y, targ.X));
				player2Transform->Rotation = angle;
			}
		}

		// Projectile inputs.
		if (window.GetInput().GetKeyDown(Key::Space) && Player1ProjectileFireRate.CanFire() && Player1ProjectileCount < MaxActiveProjectiles)
		{
			SpawnProjectile(Player1Handle, Player1ProjectileLayerName, LayerMask::GetMask({ Player2LayerName, BoundaryLayerName }));
			Player1ProjectileFireRate.Consume();

			Player1ProjectileCount++;
		}

		if (IsMultiplayer)
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
			// --- Player 2 attack AI ---

			// Attack when in range.
			if (Player2ProjectileFireRate.CanFire() && Player2ProjectileCount < MaxActiveProjectiles)
			{
				auto player1Transform = scene.Transforms.TryGetByOwner(Player1Handle);
				auto player2Transform = scene.Transforms.TryGetByOwnerMutable(Player2Handle);

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
		auto player1Transform = scene.Transforms.TryGetByOwnerMutable(Player1Handle);
		if (player1Transform)
		{
			player1Transform->Rotation += player1Turn * PlayerRotationSpeed * Timer;
			player1Transform->Position += player1Transform->Forward() * (player1Thrust * Player1Speed * Timer);
		}

		auto player2Transform = scene.Transforms.TryGetByOwnerMutable(Player2Handle);
		if (player2Transform)
		{
			player2Transform->Rotation += player2Turn * PlayerRotationSpeed * Timer;
			player2Transform->Position += player2Transform->Forward() * (player2Thrust * Player2Speed * Timer);
		}

		// Projectile movement.
		for (auto& projectileOwnerHandle : Projectiles)
		{
			auto projectileTransform = scene.Transforms.TryGetByOwnerMutable(projectileOwnerHandle);
			if (projectileTransform)
			{
				auto velocity = projectileTransform->Forward() * ProjectileSpeed;
				projectileTransform->Position += velocity * Timer;
			}
		}

#pragma endregion

#pragma region Collision Handling

		collisionSystem.Step(scene.Colliders, scene.Transforms);

		const LayerMask maskPlayer1 = LayerMask::NameToLayer(Player1LayerName);
		const LayerMask maskPlayer2 = LayerMask::NameToLayer(Player2LayerName);
		const LayerMask maskPlayer1Projectile = LayerMask::NameToLayer(Player1ProjectileLayerName);
		const LayerMask maskPlayer2Projectile = LayerMask::NameToLayer(Player2ProjectileLayerName);
		const LayerMask maskBoundary = LayerMask::NameToLayer(BoundaryLayerName);

		for (auto& collision : collisionSystem.GetCollisionInfo())
		{
			auto ownerHandleA = collision.CollisionAOwnerHandle;
			auto ownerHandleB = collision.CollisionBOwnerHandle;

			auto colliderA = scene.Colliders.TryGetByOwner(ownerHandleA);
			auto colliderB = scene.Colliders.TryGetByOwner(ownerHandleB);

			if (!colliderA || !colliderB)
			{
				continue;
			}

			auto maskA = LayerMask::FromIndex(colliderA->LayerIndex);
			auto maskB = LayerMask::FromIndex(colliderB->LayerIndex);

			auto transformA = scene.Transforms.TryGetByOwnerMutable(ownerHandleA);
			auto transformB = scene.Transforms.TryGetByOwnerMutable(ownerHandleB);
			if (!transformA || !transformB)
			{
				continue;
			}

			auto correction = collision.Point.Normal * collision.Point.Penetration;

			collisionSystem.ResolveCollision(transformA, transformB, colliderA->IsStatic, colliderB->IsStatic, correction);

			if ((maskA == maskPlayer2 && maskB == maskPlayer1Projectile) || (maskA == maskPlayer1Projectile && maskB == maskPlayer2))
			{
				std::cout << "Player 2 hit by a Player 1's projectile!" << std::endl;

				scene.DestroyGameObject(ownerHandleA);
				scene.DestroyGameObject(ownerHandleB);

				std::erase(Projectiles, maskA == maskPlayer1Projectile ? ownerHandleA : ownerHandleB);

				Player1ProjectileCount = 0;
				Player1ProjectileBounceCount = 0;
			}

			if ((maskA == maskPlayer1 && maskB == maskPlayer2Projectile) || (maskA == maskPlayer2Projectile && maskB == maskPlayer1))
			{
				std::cout << "Player 1 hit by Player 2's projectile!" << std::endl;

				scene.DestroyGameObject(ownerHandleA);
				scene.DestroyGameObject(ownerHandleB);

				std::erase(Projectiles, maskA == maskPlayer2Projectile ? ownerHandleA : ownerHandleB);

				Player2ProjectileCount = 0;
				Player2ProjectileBounceCount = 0;
			}

			if ((maskA == maskBoundary && maskB == maskPlayer1Projectile) || (maskA == maskPlayer1Projectile && maskB == maskBoundary))
			{
				std::cout << "Player 1's projectile collided with a boundary!" << std::endl;

				auto ownerHandle = maskA == maskPlayer1Projectile ? ownerHandleA : ownerHandleB;
				auto transform = scene.Transforms.TryGetByOwnerMutable(ownerHandle);
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
					scene.DestroyGameObject(ownerHandle);
					std::erase(Projectiles, ownerHandle);

					Player1ProjectileBounceCount = 0;
					Player1ProjectileCount = 0;
				}
			}

			if ((maskA == maskBoundary && maskB == maskPlayer2Projectile) || (maskA == maskPlayer2Projectile && maskB == maskBoundary))
			{
				std::cout << "Player 2's projectile collided with a boundary!" << std::endl;

				auto ownerHandle = maskA == maskPlayer2Projectile ? ownerHandleA : ownerHandleB;
				auto transform = scene.Transforms.TryGetByOwnerMutable(ownerHandle);
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
					scene.DestroyGameObject(ownerHandle);
					std::erase(Projectiles, ownerHandle);

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
