#pragma once

namespace Pong::Models
{
	struct RuntimeData
	{
		Ludus::Math::Vector2D BallPosition;
		Ludus::Math::Vector2D Player1Position;
		Ludus::Math::Vector2D Player2Position;

		float BallSpeed = 0.0f;
		bool IsBallServed = false;
		bool IsRunning = false;
	};
}
