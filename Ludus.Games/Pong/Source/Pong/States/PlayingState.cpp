#include <Pong/States/PlayingState.h>

#include <Ludus/Debug/Debug.h>

namespace Pong::States
{

#pragma region Helpers

	void PlayingState::Start()
	{
		auto directionX = m_GameInfo.Random.NextFloat(-1.0f, 1.0f);
		directionX = directionX < 0.0f ? -1.0f : 1.0f;
		auto directionY = m_GameInfo.Random.NextFloat(-0.5f, 0.5f);

		if (auto ballTransform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle))
		{
			ballTransform->Rotation = Ludus::Math::Numeric::RadiansToDegrees(std::atan2(directionY, directionX));
		}

		m_RuntimeData.IsBallServed = false;
		m_RuntimeData.IsRunning = true;
	}

	void PlayingState::Clear()
	{
		if (auto ballTransform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle))
		{
			ballTransform->Position = { m_RenderData.GetHalfWidth(), m_RenderData.GetHalfHeight() };
		}

		if (auto player1Transform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.Player1Handle))
		{
			player1Transform->Position = { Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() };
		}

		if (auto player2Transform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.Player2Handle))
		{
			player2Transform->Position = { m_RenderData.Width - Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() };
		}

		m_RuntimeData.BallSpeed = Pong::Core::Configuration::Defaults::BallSpeedDefault;
		m_RuntimeData.IsRunning = false;
	}

	static bool Is(const Ludus::Engine::LayerMask& a, const Ludus::Engine::LayerMask& b) { return a == b; };
	static bool IsPair(const Ludus::Engine::LayerMask& a1, const Ludus::Engine::LayerMask& b1, const Ludus::Engine::LayerMask& a2, const Ludus::Engine::LayerMask& b2)
	{
		return (a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2);
	};

	static float GetReflectionAngle(
		const Ludus::Math::Transform2D* ballTransform,
		const Ludus::Math::Transform2D* playerTransform,
		Ludus::Math::Vector2D normal,
		float minCenterDegrees,
		float maxDeflectDegrees
	)
	{
		// Compute a signed offset in [-1,1] from the vertical paddle impact position.
		auto offset = (ballTransform->Position.Y - playerTransform->Position.Y) / (playerTransform->Scale.Y * 0.5f);
		offset = Ludus::Math::Numeric::Clamp(offset, -1.0f, 1.0f);

		// Reflect the direction off the surface normal.
		const auto direction = ballTransform->Forward();
		const auto reflection = Ludus::Math::Vector2D::Reflect(direction, normal);

		const float theta = std::copysign(
			std::max(minCenterDegrees, std::fabs(offset) * maxDeflectDegrees),
			(offset != 0.0f) ? offset : (direction.Y >= 0.0f ? 1.0f : -1.0f)
		);
		auto deflection = Ludus::Math::Vector2D::Rotate(reflection, theta);

		// Make sure that the ball will not deflect completely vertical.
		if (std::fabs(deflection.X) < 0.1f)
		{
			deflection.X = deflection.X < 0.0f ? -0.1f : 0.1f;
		}

		return Ludus::Math::Numeric::RotationDegreesFromDirection(deflection.X, deflection.Y);
	}

#pragma endregion

	void PlayingState::Init()
	{
		// Defaults.
		m_RenderData.Width = m_GameInfo.WindowOptions.Width;
		m_RenderData.Height = m_GameInfo.WindowOptions.Height;
		m_RuntimeData.BallSpeed = Pong::Core::Configuration::Defaults::BallSpeedDefault;

		// Layers.
		using namespace Pong::Core::Layers;

		RegisterAllLayers();

		m_LayerIndexBall = ToIndex(Layer::Ball);
		m_LayerIndexHorizontal = ToIndex(Layer::BoundaryHorizontal);
		m_LayerIndexVertical = ToIndex(Layer::BoundaryVertical);
		m_LayerIndexPlayer1 = ToIndex(Layer::Player1);
		m_LayerIndexPlayer2 = ToIndex(Layer::Player2);

		m_LayerMaskBall = Ludus::Engine::LayerMask::FromIndex(m_LayerIndexBall);
		m_LayerMaskHorizontal = Ludus::Engine::LayerMask::FromIndex(m_LayerIndexHorizontal);
		m_LayerMaskVertical = Ludus::Engine::LayerMask::FromIndex(m_LayerIndexVertical);
		m_LayerMaskPlayer1 = Ludus::Engine::LayerMask::FromIndex(m_LayerIndexPlayer1);
		m_LayerMaskPlayer2 = Ludus::Engine::LayerMask::FromIndex(m_LayerIndexPlayer2);

		// Game objects.
		m_Entities.BallHandle = m_GameInfo.EntityComponentSystem.AddEntity();
		m_Entities.LeftWallHandle = m_GameInfo.EntityComponentSystem.AddEntity();
		m_Entities.TopWallHandle = m_GameInfo.EntityComponentSystem.AddEntity();
		m_Entities.RightWallHandle = m_GameInfo.EntityComponentSystem.AddEntity();
		m_Entities.BottomWallHandle = m_GameInfo.EntityComponentSystem.AddEntity();
		m_Entities.Player1Handle = m_GameInfo.EntityComponentSystem.AddEntity();
		m_Entities.Player2Handle = m_GameInfo.EntityComponentSystem.AddEntity();

		// Colliders.
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.BallHandle, m_LayerIndexBall, (m_LayerMaskVertical | m_LayerMaskHorizontal | m_LayerMaskPlayer1 | m_LayerMaskPlayer2));
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.TopWallHandle, m_LayerIndexHorizontal, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2), true);
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.BottomWallHandle, m_LayerIndexHorizontal, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2), true);
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.LeftWallHandle, m_LayerIndexVertical, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2), true);
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.RightWallHandle, m_LayerIndexVertical, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2), true);
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.Player1Handle, m_LayerIndexPlayer1, (m_LayerMaskBall | m_LayerMaskHorizontal));
		m_GameInfo.EntityComponentSystem.AttachCollider(m_Entities.Player2Handle, m_LayerIndexPlayer2, (m_LayerMaskBall | m_LayerMaskHorizontal));

		// Transforms.
		const float wallWidth = Pong::Core::Configuration::Defaults::WallWidthThickness;
		const float wallHeight = Pong::Core::Configuration::Defaults::WallHeightThickness;

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.BallHandle,
			{ m_RenderData.GetHalfWidth(), m_RenderData.GetHalfHeight() },
			Pong::Core::Configuration::Defaults::BallSize);

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.TopWallHandle,
			{ m_RenderData.GetHalfWidth(), (float)m_RenderData.Height - wallHeight * 0.5f },
			{ (float)m_RenderData.Width, wallHeight });

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.BottomWallHandle,
			{ m_RenderData.GetHalfWidth(), wallHeight * 0.5f },
			{ (float)m_RenderData.Width, wallHeight });

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.LeftWallHandle,
			{ wallWidth * 0.5f, m_RenderData.GetHalfHeight() },
			{ wallWidth, (float)m_RenderData.Height });

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.RightWallHandle,
			{ (float)m_RenderData.Width - wallWidth * 0.5f, m_RenderData.GetHalfHeight() },
			{ wallWidth, (float)m_RenderData.Height });

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.Player1Handle,
			{ Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() },
			{ Pong::Core::Configuration::Defaults::PaddleWidth, Pong::Core::Configuration::Defaults::PaddleHeight });

		m_GameInfo.EntityComponentSystem.AttachTransform(
			m_Entities.Player2Handle,
			{ (float)m_RenderData.Width - Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() },
			{ Pong::Core::Configuration::Defaults::PaddleWidth, Pong::Core::Configuration::Defaults::PaddleHeight });
	}

	void PlayingState::Reset()
	{ }

	GameState PlayingState::HandleInput(Ludus::Platform::Input& input, float deltaTime)
	{
		if (m_PongInfo.Player1Score == m_PongInfo.MaxScore || m_PongInfo.Player2Score == m_PongInfo.MaxScore)
		{
			return GameState::Score;
		}

		if ((input.GetKeyDown(Ludus::Platform::Key::Enter) || input.GetKeyDown(Ludus::Platform::Key::Space)))
		{
			Start();
		}

		if (input.GetKeyDown(Ludus::Platform::Key::Escape))
		{
			return GameState::Paused;
		}

		// Player 1.
		m_Intents.Player1MoveY = (input.GetKey(Ludus::Platform::Key::W) ? 1.0f : 0.0f) + (input.GetKey(Ludus::Platform::Key::S) ? -1.0f : 0.0f);

		// Player 2.
		if (m_PongInfo.IsMultiplayer)
		{
			m_Intents.Player2MoveY = (input.GetKey(Ludus::Platform::Key::Up) ? 1.0f : 0.0f) + (input.GetKey(Ludus::Platform::Key::Down) ? -1.0f : 0.0f);
		}

		return GameState::Playing;
	}

	GameState PlayingState::Update(float deltaTime)
	{
		auto* player1Ptr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.Player1Handle);
		auto* player2Ptr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.Player2Handle);
		auto* ballPtr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle);

		if (!(player1Ptr && player2Ptr && ballPtr))
		{
			LUDUS_LOG_DEBUG("[Movement Integration] Missing transform(s).");
			return GameState::Exit;
		}

		auto& player1Transform = *player1Ptr;
		auto& player2Transform = *player2Ptr;
		auto& ballTransform = *ballPtr;

		// Player 1.
		player1Transform.Position.Y += m_Intents.Player1MoveY * Pong::Core::Configuration::Defaults::Player1Speed * deltaTime;

		// Player 2.
		if (m_RuntimeData.IsRunning && !m_PongInfo.IsMultiplayer)
		{
			// Player 2 AI.
			const float centerX = m_RenderData.GetHalfWidth();
			const float margin = 4.0f;
			const float deadzone = Pong::Core::Configuration::Defaults::BallSize;

			const auto dir = ballTransform.Forward();
			const bool isInsideBounds = (ballTransform.Position.X > (centerX + margin));
			const bool isGoingRight = (dir.X > 0.0f);
			if (isInsideBounds && isGoingRight)
			{
				const float deltaY = ballTransform.Position.Y - player2Transform.Position.Y;

				if (std::fabs(deltaY) > deadzone)
				{
					m_Intents.Player2MoveY = (deltaY > 0.0f) ? 1.0f : -1.0f;
				}
				else
				{
					m_Intents.Player2MoveY = 0.0f;
				}
			}
			else
			{
				m_Intents.Player2MoveY = 0.0f;
			}
		}

		player2Transform.Position.Y += m_Intents.Player2MoveY * Pong::Core::Configuration::Defaults::Player2Speed * deltaTime;

		// Ball.
		if (m_RuntimeData.IsRunning)
		{
			const auto ballVelocity = ballTransform.Forward() * (m_RuntimeData.IsBallServed ? m_RuntimeData.BallSpeed : m_RuntimeData.BallSpeed * 0.5f);
			ballTransform.Position += ballVelocity * deltaTime;
		}

		m_GameInfo.CollisionSystem.Step(m_GameInfo.EntityComponentSystem.Colliders, m_GameInfo.EntityComponentSystem.Transforms);

		const auto& collisionInfo = m_GameInfo.CollisionSystem.GetCollisionInfo();
		for (const auto& info : collisionInfo)
		{
			const auto ownerHandleA = info.CollisionAOwnerHandle;
			const auto ownerHandleB = info.CollisionBOwnerHandle;

			const auto& contactPoint = info.Point;

			const auto* colliderAPtr = m_GameInfo.EntityComponentSystem.Colliders.TryGetByOwnerMutable(ownerHandleA);
			const auto* colliderBPtr = m_GameInfo.EntityComponentSystem.Colliders.TryGetByOwnerMutable(ownerHandleB);

			if (!(colliderAPtr && colliderBPtr))
			{
				LUDUS_LOG_DEBUG("[Collision Handling] Missing transform(s).");
				continue;
			}

			const auto& colliderA = *colliderAPtr;
			const auto& colliderB = *colliderBPtr;

			auto* transformAPtr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(ownerHandleA);
			auto* transformBPtr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(ownerHandleB);

			if (!(transformAPtr && transformBPtr))
			{
				LUDUS_LOG_DEBUG("[Collision Handling] Missing colliders(s).");
				continue;
			}

			auto& transformA = *transformAPtr;
			auto& transformB = *transformBPtr;

			const auto maskA = Ludus::Engine::LayerMask::FromIndex(colliderA.LayerIndex);
			const auto maskB = Ludus::Engine::LayerMask::FromIndex(colliderB.LayerIndex);

			if (!IsPair(maskA, maskB, m_LayerMaskBall, m_LayerMaskVertical))
			{
				const auto isAStatic = Is(maskB, m_LayerMaskBall) && (Is(maskA, m_LayerMaskPlayer1) || Is(maskA, m_LayerMaskPlayer2)) ? true : colliderA.IsStatic;
				const auto isBStatic = Is(maskA, m_LayerMaskBall) && (Is(maskB, m_LayerMaskPlayer1) || Is(maskB, m_LayerMaskPlayer2)) ? true : colliderB.IsStatic;
				const auto correction = contactPoint.Normal * contactPoint.Penetration;
				m_GameInfo.CollisionSystem.ResolveCollision(transformAPtr, transformBPtr, isAStatic, isBStatic, correction);
			}

			auto* ballTransform =
				(colliderA.LayerIndex == m_LayerIndexBall) ? &transformA :
				(colliderB.LayerIndex == m_LayerIndexBall) ? &transformB : nullptr;

			if (ballTransform)
			{
				if (IsPair(maskA, maskB, m_LayerMaskBall, m_LayerMaskHorizontal))
				{
					LUDUS_LOG_DEBUG("[Collision Handling] Ball <-> Boundary (horizontal)");

					const Ludus::Math::Vector2D normal = (ballTransform->Position.Y > m_RenderData.GetHalfHeight() ? Ludus::Math::Vector2D(0.0f, -1.0f) : Ludus::Math::Vector2D(0.0f, 1.0f));
					if (Ludus::Math::Vector2D::Dot(ballTransform->Forward(), normal) < 0.0f)
					{
						const auto reflectionVector = Ludus::Math::Vector2D::Reflect(ballTransform->Forward(), normal);
						ballTransform->Rotate(reflectionVector);
						ballTransform->Position += normal * Ludus::Physics::Constants::Collisions::SEPARATION_NUDGE_FACTOR;
					}
				}
				else if (IsPair(maskA, maskB, m_LayerMaskBall, m_LayerMaskPlayer1))
				{
					LUDUS_LOG_DEBUG("[Collision Handling] Ball <-> Player 1");

					const auto& player1Transform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwner(m_Entities.Player1Handle);
					if (!player1Transform)
					{
						LUDUS_LOG_DEBUG("[Collision Handling] Missing transform(s).");
						continue;
					}

					const Ludus::Math::Vector2D normal = { 1.0f, 0.0f };
					if (Ludus::Math::Vector2D::Dot(ballTransform->Forward(), normal) < 0.0f)
					{
						const auto reflectionAngle = GetReflectionAngle(
							ballTransform,
							player1Transform,
							normal,
							Pong::Core::Configuration::Defaults::MinCenterDegrees,
							Pong::Core::Configuration::Defaults::MaxDeflectDegrees
						);
						ballTransform->Rotation = reflectionAngle;
						ballTransform->Position += normal * Ludus::Physics::Constants::Collisions::SEPARATION_NUDGE_FACTOR;
					}
				}
				else if (IsPair(maskA, maskB, m_LayerMaskBall, m_LayerMaskPlayer2))
				{
					LUDUS_LOG_DEBUG("[Collision Handling] Ball <-> Player 2");

					const auto& player2Transform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwner(m_Entities.Player2Handle);
					if (!player2Transform)
					{
						LUDUS_LOG_DEBUG("[Collision Handling] Missing transform(s).");
						continue;
					}

					const Ludus::Math::Vector2D normal = { -1.0f, 0.0f };
					if (Ludus::Math::Vector2D::Dot(ballTransform->Forward(), normal) < 0.0f)
					{
						const auto reflectionAngle = GetReflectionAngle(
							ballTransform,
							player2Transform,
							normal,
							Pong::Core::Configuration::Defaults::MinCenterDegrees,
							Pong::Core::Configuration::Defaults::MaxDeflectDegrees
						);
						ballTransform->Rotation = reflectionAngle;
						ballTransform->Position += normal * Ludus::Physics::Constants::Collisions::SEPARATION_NUDGE_FACTOR;
					}
				}
				else
				{
					LUDUS_LOG_DEBUG("[Collision Handling] Ball <-> Boundary (vertical)");

					if (ballTransform->Position.X <= Pong::Core::Configuration::Defaults::WallWidthThickness)
					{
						m_PongInfo.Player2Score++;
						Clear();
					}
					else if (ballTransform->Position.X >= m_RenderData.Width - Pong::Core::Configuration::Defaults::WallWidthThickness)
					{
						m_PongInfo.Player1Score++;
						Clear();
					}
				}

				m_RuntimeData.IsBallServed = true;
			}
			else
			{
				if (IsPair(maskA, maskB, m_LayerMaskPlayer1, m_LayerMaskHorizontal))
				{
					LUDUS_LOG_DEBUG("[Collision Handling] Player 1 <-> Boundary (horizontal)");
				}
				else
				{
					LUDUS_LOG_DEBUG("[Collision Handling] Player 2 <-> Boundary (horizontal)");
				}
			}
		}

		return GameState::Playing;
	}

	void PlayingState::Render(float deltaTime)
	{
		// Render stippled center line.
		for (float y = 0; y < m_RenderData.Height; y += m_RenderData.GetLineHeight())
		{
			m_GameInfo.Renderer.DrawLine(m_RenderData.GetHalfWidth(), y, m_RenderData.GetHalfWidth(), y + m_RenderData.GetHalfLineHeight());
		}

		// Render score text.
		auto color = Ludus::Graphics::Colors::White;
		auto horizontalAlignment = Ludus::Graphics::HorizontalTextAlignment::Center;

		m_GameInfo.Renderer.DrawText(
			Ludus::Math::Transform2D(0, { m_RenderData.GetHalfWidth() - Pong::Core::Configuration::Defaults::ScoreTextOffset, m_RenderData.Height - Pong::Core::Configuration::Defaults::ScoreTextOffset }),
			std::to_string(m_PongInfo.Player1Score),
			color,
			horizontalAlignment
		);
		m_GameInfo.Renderer.DrawText(
			Ludus::Math::Transform2D(0, { m_RenderData.GetHalfWidth() + Pong::Core::Configuration::Defaults::ScoreTextOffset, m_RenderData.Height - Pong::Core::Configuration::Defaults::ScoreTextOffset }),
			std::to_string(m_PongInfo.Player2Score),
			color,
			horizontalAlignment
		);

		// Render top and bottom boundaries.
		const auto* boundaryTopTransform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.TopWallHandle);
		const auto* boundaryBottomTransform = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.BottomWallHandle);
		if (!(boundaryTopTransform && boundaryBottomTransform))
		{
			LUDUS_LOG_DEBUG("[Rendering] Missing transform(s).");
			return;
		}

		m_GameInfo.Renderer.DrawQuad(*boundaryTopTransform);
		m_GameInfo.Renderer.DrawQuad(*boundaryBottomTransform);

		const auto* player1Ptr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.Player1Handle);
		const auto* player2Ptr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.Player2Handle);
		const auto* ballPtr = m_GameInfo.EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle);

		if (!(player1Ptr && player2Ptr && ballPtr))
		{
			LUDUS_LOG_DEBUG("[Rendering] Missing transform(s).");
			return;
		}

		// Render ball and players.
		m_GameInfo.Renderer.DrawQuad(*player1Ptr);
		m_GameInfo.Renderer.DrawQuad(*player2Ptr);
		m_GameInfo.Renderer.DrawCircle(*ballPtr);
	}
}
