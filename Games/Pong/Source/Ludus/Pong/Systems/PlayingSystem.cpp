#include <Ludus/Pong/Systems/PlayingSystem.h>

namespace Ludus::Pong::Systems
{
	PlayingSystem::PlayingSystem(Ludus::Pong::Core::GameInfo& gameInfo, Ludus::Pong::Core::PongInfo& pongInfo)
		: m_GameInfo(gameInfo),
		m_PongInfo(pongInfo),
		m_LayerIndexBall(-1),
		m_LayerIndexHorizontal(-1),
		m_LayerIndexVertical(-1),
		m_LayerIndexPlayer1(-1),
		m_LayerIndexPlayer2(-1),
		m_LayerMaskBall(Ludus::Engine::Physics::Core::LayerMask::GetEmpty()),
		m_LayerMaskHorizontal(Ludus::Engine::Physics::Core::LayerMask::GetEmpty()),
		m_LayerMaskVertical(Ludus::Engine::Physics::Core::LayerMask::GetEmpty()),
		m_LayerMaskPlayer1(Ludus::Engine::Physics::Core::LayerMask::GetEmpty()),
		m_LayerMaskPlayer2(Ludus::Engine::Physics::Core::LayerMask::GetEmpty())
	{ }

	void PlayingSystem::Start()
	{
		auto directionX = m_GameInfo.Random.NextFloat(-1.0f, 1.0f);
		directionX = directionX < 0.0f ? -1.0f : 1.0f;
		auto directionY = m_GameInfo.Random.NextFloat(-0.5f, 0.5f);

		if (auto ballTransform = m_SystemContext->EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle))
		{
			ballTransform->Rotation = Ludus::Engine::Math::Numeric::RadiansToDegrees(std::atan2(directionY, directionX));
		}

		m_RuntimeData.IsBallServed = false;
		m_RuntimeData.IsRunning = true;
	}

	void PlayingSystem::Clear()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		if (auto ballTransform = ecs.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle))
		{
			ballTransform->Position = { m_RenderData.GetHalfWidth(), m_RenderData.GetHalfHeight() };
		}

		if (auto player1Transform = ecs.Transforms.TryGetByOwnerMutable(m_Entities.Player1Handle))
		{
			player1Transform->Position = { Ludus::Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() };
		}

		if (auto player2Transform = ecs.Transforms.TryGetByOwnerMutable(m_Entities.Player2Handle))
		{
			player2Transform->Position = { m_RenderData.Width - Ludus::Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() };
		}

		m_RuntimeData.BallSpeed = Ludus::Pong::Core::Configuration::Defaults::BallSpeedDefault;
		m_RuntimeData.IsRunning = false;
	}

	static float GetReflectionAngle(
		const Ludus::Engine::Math::Transform2D& ballTransform,
		const Ludus::Engine::Math::Transform2D& playerTransform,
		Ludus::Engine::Math::Vector2D normal,
		float minCenterDegrees,
		float maxDeflectDegrees
	)
	{
		// Compute a signed offset in [-1,1] from the vertical paddle impact position.
		auto offset = (ballTransform.Position.Y - playerTransform.Position.Y) / (playerTransform.Scale.Y * 0.5f);
		offset = Ludus::Engine::Math::Numeric::Clamp(offset, -1.0f, 1.0f);

		// Reflect the direction off the surface normal.
		const auto direction = ballTransform.Forward();
		const auto reflection = Ludus::Engine::Math::Vector2D::Reflect(direction, normal);

		const float theta = std::copysign(
			std::max(minCenterDegrees, std::fabs(offset) * maxDeflectDegrees),
			(offset != 0.0f) ? offset : (direction.Y >= 0.0f ? 1.0f : -1.0f)
		);
		auto deflection = Ludus::Engine::Math::Vector2D::Rotate(reflection, theta);

		// Make sure that the ball will not deflect completely vertical.
		if (std::fabs(deflection.X) < 0.1f)
		{
			deflection.X = deflection.X < 0.0f ? -0.1f : 0.1f;
		}

		return Ludus::Engine::Math::Numeric::RotationDegreesFromDirection(deflection.X, deflection.Y);
	}

	void PlayingSystem::OnAttachImpl()
	{
		// Defaults.
		auto& options = m_SystemContext->Window.GetOptions();
		m_RenderData.Width = options.Width;
		m_RenderData.Height = options.Height;

		m_RenderData.DefaultBallPosition = { m_RenderData.GetHalfWidth(), m_RenderData.GetHalfHeight() };
		m_RenderData.DefaultPlayer1Position = { Ludus::Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() };
		m_RenderData.DefaultPlayer2Position = { (float)m_RenderData.Width - Ludus::Pong::Core::Configuration::Defaults::PaddleXOffset, m_RenderData.GetHalfHeight() };

		m_RuntimeData.BallPosition = m_RenderData.DefaultBallPosition;
		m_RuntimeData.Player1Position = m_RenderData.DefaultPlayer1Position;
		m_RuntimeData.Player2Position = m_RenderData.DefaultPlayer2Position;

		m_RuntimeData.BallSpeed = Ludus::Pong::Core::Configuration::Defaults::BallSpeedDefault;

		// Layers.
		using namespace Ludus::Pong::Core::Layers;

		RegisterAllLayers();

		m_LayerIndexBall = ToIndex(Layer::Ball);
		m_LayerIndexHorizontal = ToIndex(Layer::BoundaryHorizontal);
		m_LayerIndexVertical = ToIndex(Layer::BoundaryVertical);
		m_LayerIndexPlayer1 = ToIndex(Layer::Player1);
		m_LayerIndexPlayer2 = ToIndex(Layer::Player2);

		m_LayerMaskBall = Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_LayerIndexBall);
		m_LayerMaskHorizontal = Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_LayerIndexHorizontal);
		m_LayerMaskVertical = Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_LayerIndexVertical);
		m_LayerMaskPlayer1 = Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_LayerIndexPlayer1);
		m_LayerMaskPlayer2 = Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_LayerIndexPlayer2);
	}

	void PlayingSystem::OnDetachImpl()
	{
		OnTransitionExitImpl();
	}

	void PlayingSystem::OnTransitionEnterImpl()
	{
		LUDUS_LOG_DEBUG("ENTERING PLAYING STATE");

		auto& ecs = m_SystemContext->EntityComponentSystem;

		// Game objects.
		m_Entities.BallHandle = ecs.AddEntity();
		m_Entities.LeftWallHandle = ecs.AddEntity();
		m_Entities.TopWallHandle = ecs.AddEntity();
		m_Entities.RightWallHandle = ecs.AddEntity();
		m_Entities.BottomWallHandle = ecs.AddEntity();
		m_Entities.Player1Handle = ecs.AddEntity();
		m_Entities.Player2Handle = ecs.AddEntity();

		// Colliders.
		ecs.AttachCollider(m_Entities.BallHandle, m_LayerIndexBall, (m_LayerMaskVertical | m_LayerMaskHorizontal | m_LayerMaskPlayer1 | m_LayerMaskPlayer2));
		ecs.AttachCollider(m_Entities.TopWallHandle, m_LayerIndexHorizontal, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2));
		ecs.AttachCollider(m_Entities.BottomWallHandle, m_LayerIndexHorizontal, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2));
		ecs.AttachCollider(m_Entities.LeftWallHandle, m_LayerIndexVertical, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2), true);
		ecs.AttachCollider(m_Entities.RightWallHandle, m_LayerIndexVertical, (m_LayerMaskBall | m_LayerMaskPlayer1 | m_LayerMaskPlayer2), true);
		ecs.AttachCollider(m_Entities.Player1Handle, m_LayerIndexPlayer1, (m_LayerMaskBall | m_LayerMaskHorizontal));
		ecs.AttachCollider(m_Entities.Player2Handle, m_LayerIndexPlayer2, (m_LayerMaskBall | m_LayerMaskHorizontal));

		// Rigid Bodies.
		ecs.AttachRigidBody(m_Entities.BallHandle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Dynamic, 0.0f);
		ecs.AttachRigidBody(m_Entities.TopWallHandle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachRigidBody(m_Entities.BottomWallHandle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachRigidBody(m_Entities.LeftWallHandle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachRigidBody(m_Entities.RightWallHandle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachRigidBody(m_Entities.Player1Handle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Kinematic);
		ecs.AttachRigidBody(m_Entities.Player2Handle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Kinematic);

		// Transforms.
		const float wallWidth = Ludus::Pong::Core::Configuration::Defaults::WallWidthThickness;
		const float wallHeight = Ludus::Pong::Core::Configuration::Defaults::WallHeightThickness;

		ecs.AttachTransform(
			m_Entities.BallHandle,
			m_RuntimeData.BallPosition,
			Ludus::Pong::Core::Configuration::Defaults::BallSize);

		ecs.AttachTransform(
			m_Entities.TopWallHandle,
			{ m_RenderData.GetHalfWidth(), (float)m_RenderData.Height - wallHeight * 0.5f },
			{ (float)m_RenderData.Width, wallHeight });

		ecs.AttachTransform(
			m_Entities.BottomWallHandle,
			{ m_RenderData.GetHalfWidth(), wallHeight * 0.5f },
			{ (float)m_RenderData.Width, wallHeight });

		ecs.AttachTransform(
			m_Entities.LeftWallHandle,
			{ wallWidth * 0.5f, m_RenderData.GetHalfHeight() },
			{ wallWidth, (float)m_RenderData.Height });

		ecs.AttachTransform(
			m_Entities.RightWallHandle,
			{ (float)m_RenderData.Width - wallWidth * 0.5f, m_RenderData.GetHalfHeight() },
			{ wallWidth, (float)m_RenderData.Height });

		ecs.AttachTransform(
			m_Entities.Player1Handle,
			m_RuntimeData.Player1Position,
			{ Ludus::Pong::Core::Configuration::Defaults::PaddleWidth, Ludus::Pong::Core::Configuration::Defaults::PaddleHeight });

		ecs.AttachTransform(
			m_Entities.Player2Handle,
			m_RuntimeData.Player2Position,
			{ Ludus::Pong::Core::Configuration::Defaults::PaddleWidth, Ludus::Pong::Core::Configuration::Defaults::PaddleHeight });

		// Transforms.
		ecs.AttachSprite(m_Entities.BallHandle, Ludus::Engine::Graphics::Shape::Circle);
		ecs.AttachSprite(m_Entities.TopWallHandle);
		ecs.AttachSprite(m_Entities.BottomWallHandle);
		ecs.AttachSprite(m_Entities.Player1Handle);
		ecs.AttachSprite(m_Entities.Player2Handle);

		// Render stippled center line.
		for (float y = 0; y < m_RenderData.Height; y += m_RenderData.GetLineHeight())
		{
			auto handle = ecs.AddEntity();
			ecs.AttachTransform(handle, { m_RenderData.GetHalfWidth(), y }, { m_RenderData.GetLineWidth(), m_RenderData.GetHalfLineHeight() });
			ecs.AttachSprite(handle);
			m_Entities.CenterLineHandles.push_back(handle);
		}

		// Render score text.
		auto color = Ludus::Engine::Graphics::Colors::White;
		auto horizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Center;

		m_Entities.LeftScoreTextHandle = ecs.AddEntity();
		ecs.AttachTransform(m_Entities.LeftScoreTextHandle, { m_RenderData.GetHalfWidth() - Ludus::Pong::Core::Configuration::Defaults::ScoreTextOffset, m_RenderData.Height - Ludus::Pong::Core::Configuration::Defaults::ScoreTextOffset });
		ecs.AttachText(m_Entities.LeftScoreTextHandle, std::to_string(m_PongInfo.Player1Score), color, horizontalAlignment);

		m_Entities.RightScoreTextHandle = ecs.AddEntity();
		ecs.AttachTransform(m_Entities.RightScoreTextHandle, { m_RenderData.GetHalfWidth() + Ludus::Pong::Core::Configuration::Defaults::ScoreTextOffset, m_RenderData.Height - Ludus::Pong::Core::Configuration::Defaults::ScoreTextOffset });
		ecs.AttachText(m_Entities.RightScoreTextHandle, std::to_string(m_PongInfo.Player2Score), color, horizontalAlignment);
	}

	void PlayingSystem::OnTransitionExitImpl()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		auto* ballPtr = ecs.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle);
		auto* player1Ptr = ecs.Transforms.TryGetByOwnerMutable(m_Entities.Player1Handle);
		auto* player2Ptr = ecs.Transforms.TryGetByOwnerMutable(m_Entities.Player2Handle);

		if (!(ballPtr && player1Ptr && player2Ptr))
		{
			LUDUS_LOG_ERROR("Missing transform(s).");
			return;
		}

		m_RuntimeData.BallPosition = ballPtr->Position;
		m_RuntimeData.Player1Position = player1Ptr->Position;
		m_RuntimeData.Player2Position = player2Ptr->Position;

		ecs.DestroyEntity(m_Entities.BallHandle);
		ecs.DestroyEntity(m_Entities.TopWallHandle);
		ecs.DestroyEntity(m_Entities.BottomWallHandle);
		ecs.DestroyEntity(m_Entities.LeftWallHandle);
		ecs.DestroyEntity(m_Entities.RightWallHandle);
		ecs.DestroyEntity(m_Entities.Player1Handle);
		ecs.DestroyEntity(m_Entities.Player2Handle);
		ecs.DestroyEntity(m_Entities.LeftScoreTextHandle);
		ecs.DestroyEntity(m_Entities.RightScoreTextHandle);

		for (auto handle : m_Entities.CenterLineHandles)
		{
			ecs.DestroyEntity(handle);
		}
	}

	void PlayingSystem::UpdateImpl(float deltaTime)
	{
		auto& input = m_SystemContext->Input;

		auto state = Ludus::Pong::Core::GameState::Playing;
		if (m_PongInfo.Player1Score == m_PongInfo.MaxScore || m_PongInfo.Player2Score == m_PongInfo.MaxScore)
		{
			state = Ludus::Pong::Core::GameState::ScoreMenu;
		}

		if (!m_RuntimeData.IsRunning && (input.GetKeyDown(Ludus::Engine::Platform::Key::Enter) || input.GetKeyDown(Ludus::Engine::Platform::Key::Space)))
		{
			Start();
		}

		if (input.GetKeyDown(Ludus::Engine::Platform::Key::Escape))
		{
			state = Ludus::Pong::Core::GameState::PauseMenu;
		}

		// Player 1.
		m_Intents.Player1MoveY = (input.GetKey(Ludus::Engine::Platform::Key::W) ? 1.0f : 0.0f) + (input.GetKey(Ludus::Engine::Platform::Key::S) ? -1.0f : 0.0f);

		// Player 2.
		if (m_PongInfo.IsMultiplayer)
		{
			m_Intents.Player2MoveY = (input.GetKey(Ludus::Engine::Platform::Key::Up) ? 1.0f : 0.0f) + (input.GetKey(Ludus::Engine::Platform::Key::Down) ? -1.0f : 0.0f);
		}

		if (state != Ludus::Pong::Core::GameState::Playing)
		{
			auto& gameState = m_SystemContext->Resources.Get<Ludus::Engine::Core::State<Ludus::Pong::Core::GameState>>();
			gameState.TransitionTo(state);
		}

		auto& ecs = m_SystemContext->EntityComponentSystem;

		auto* player1Ptr = ecs.Transforms.TryGetByOwnerMutable(m_Entities.Player1Handle);
		auto* player2Ptr = ecs.Transforms.TryGetByOwnerMutable(m_Entities.Player2Handle);
		auto* ballPtr = ecs.Transforms.TryGetByOwnerMutable(m_Entities.BallHandle);

		if (!(player1Ptr && player2Ptr && ballPtr))
		{
			LUDUS_LOG_ERROR("Missing transform(s).");
			return;
		}

		auto& player1Transform = *player1Ptr;
		auto& player2Transform = *player2Ptr;
		auto& ballTransform = *ballPtr;

		// Player 1.
		player1Transform.Position.Y += m_Intents.Player1MoveY * Ludus::Pong::Core::Configuration::Defaults::Player1Speed * deltaTime;

		// Player 2.
		if (m_RuntimeData.IsRunning && !m_PongInfo.IsMultiplayer)
		{
			// Player 2 AI.
			const float centerX = m_RenderData.GetHalfWidth();
			const float margin = 4.0f;
			const float deadzone = Ludus::Pong::Core::Configuration::Defaults::BallSize;

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

		player2Transform.Position.Y += m_Intents.Player2MoveY * Ludus::Pong::Core::Configuration::Defaults::Player2Speed * deltaTime;

		// Ball.
		if (m_RuntimeData.IsRunning)
		{
			const auto ballVelocity = ballTransform.Forward() * (m_RuntimeData.IsBallServed ? m_RuntimeData.BallSpeed : m_RuntimeData.BallSpeed * 0.5f);
			ballTransform.Position += ballVelocity * deltaTime;
		}

		// Collisions.
		auto& physicsQueries = m_SystemContext->GetPhysicsQueries();

		if (physicsQueries.IsColliding(m_Entities.BallHandle, m_Entities.TopWallHandle) || physicsQueries.IsColliding(m_Entities.BallHandle, m_Entities.BottomWallHandle))
		{
			LUDUS_LOG_DEBUG("Ball <-> Boundary (horizontal)");

			const Ludus::Engine::Math::Vector2D normal = (ballTransform.Position.Y > m_RenderData.GetHalfHeight() ? Ludus::Engine::Math::Vector2D(0.0f, -1.0f) : Ludus::Engine::Math::Vector2D(0.0f, 1.0f));
			if (Ludus::Engine::Math::Vector2D::Dot(ballTransform.Forward(), normal) < 0.0f)
			{
				const auto reflectionVector = Ludus::Engine::Math::Vector2D::Reflect(ballTransform.Forward(), normal);
				ballTransform.Rotate(reflectionVector);
				ballTransform.Position += normal * Ludus::Engine::Physics::Core::Constants::SEPARATION_NUDGE_FACTOR;
			}

			m_RuntimeData.IsBallServed = true;
		}
		else if (physicsQueries.IsColliding(m_Entities.BallHandle, m_Entities.Player1Handle))
		{
			LUDUS_LOG_DEBUG("Ball <-> Player 1");

			const Ludus::Engine::Math::Vector2D normal = { 1.0f, 0.0f };
			if (Ludus::Engine::Math::Vector2D::Dot(ballTransform.Forward(), normal) < 0.0f)
			{
				const auto reflectionAngle = GetReflectionAngle(
					ballTransform,
					player1Transform,
					normal,
					Ludus::Pong::Core::Configuration::Defaults::MinCenterDegrees,
					Ludus::Pong::Core::Configuration::Defaults::MaxDeflectDegrees
				);
				ballTransform.Rotation = reflectionAngle;
				ballTransform.Position += normal * Ludus::Engine::Physics::Core::Constants::SEPARATION_NUDGE_FACTOR;
			}

			m_RuntimeData.IsBallServed = true;
		}
		else if (physicsQueries.IsColliding(m_Entities.BallHandle, m_Entities.Player2Handle))
		{
			LUDUS_LOG_DEBUG("Ball <-> Player 2");

			const Ludus::Engine::Math::Vector2D normal = { -1.0f, 0.0f };
			if (Ludus::Engine::Math::Vector2D::Dot(ballTransform.Forward(), normal) < 0.0f)
			{
				const auto reflectionAngle = GetReflectionAngle(
					ballTransform,
					player2Transform,
					normal,
					Ludus::Pong::Core::Configuration::Defaults::MinCenterDegrees,
					Ludus::Pong::Core::Configuration::Defaults::MaxDeflectDegrees
				);
				ballTransform.Rotation = reflectionAngle;
				ballTransform.Position += normal * Ludus::Engine::Physics::Core::Constants::SEPARATION_NUDGE_FACTOR;
			}

			m_RuntimeData.IsBallServed = true;
		}
		else if (physicsQueries.IsColliding(m_Entities.BallHandle, m_Entities.LeftWallHandle) || physicsQueries.IsColliding(m_Entities.BallHandle, m_Entities.RightWallHandle))
		{
			LUDUS_LOG_DEBUG("Ball <-> Boundary (vertical)");

			if (ballTransform.Position.X <= Ludus::Pong::Core::Configuration::Defaults::WallWidthThickness)
			{
				m_PongInfo.Player2Score++;

				auto* rightScoreText = ecs.Texts.TryGetByOwnerMutable(m_Entities.RightScoreTextHandle);
				if (rightScoreText)
				{
					rightScoreText->Text = std::to_string(m_PongInfo.Player2Score);
				}

				Clear();
			}
			else if (ballTransform.Position.X >= m_RenderData.Width - Ludus::Pong::Core::Configuration::Defaults::WallWidthThickness)
			{
				m_PongInfo.Player1Score++;

				auto* leftScoreText = ecs.Texts.TryGetByOwnerMutable(m_Entities.LeftScoreTextHandle);
				if (leftScoreText)
				{
					leftScoreText->Text = std::to_string(m_PongInfo.Player1Score);
				}

				Clear();
			}
		}

		// Ensure that the paddles are constrained to the scene.
		const auto halfPaddleHeight = Ludus::Pong::Core::Configuration::Defaults::PaddleHeight * 0.5f;
		const auto minY = Ludus::Pong::Core::Configuration::Defaults::WallHeightThickness + halfPaddleHeight;
		const auto maxY = m_RenderData.Height - Ludus::Pong::Core::Configuration::Defaults::WallHeightThickness - halfPaddleHeight;

		player1Transform.Position.Y = Ludus::Engine::Math::Numeric::Clamp(player1Transform.Position.Y, minY, maxY);
		player2Transform.Position.Y = Ludus::Engine::Math::Numeric::Clamp(player2Transform.Position.Y, minY, maxY);
	}
}
