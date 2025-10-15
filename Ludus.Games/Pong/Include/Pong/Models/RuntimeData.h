#pragma once

namespace Pong::Models
{
	struct RuntimeData
	{
		float BallSpeed = 0.0f;
		bool IsBallServed = false;
		bool IsRunning = false;
	};
}
