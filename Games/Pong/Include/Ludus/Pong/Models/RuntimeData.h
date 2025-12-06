#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Pong::Models
{
	struct RuntimeData
	{
		Ludus::Engine::Math::Vector2D BallPosition;
		Ludus::Engine::Math::Vector2D Player1Position;
		Ludus::Engine::Math::Vector2D Player2Position;

		float BallSpeed = 0.0f;
		bool IsBallServed = false;
		bool IsRunning = false;
	};
}
