#pragma once

#include <cstdint>
#include <Ludus/Engine/LayerMask.h>

namespace Pong::Core::Configuration
{
	struct Defaults
	{
		static constexpr float PaddleWidth = 10.0f;
		static constexpr float PaddleHeight = 60.0f;
		static constexpr float PaddleXOffset = 100.0f;
		static constexpr float WallWidthThickness = 50.0f;
		static constexpr float WallHeightThickness = 15.0f;

		static constexpr float BallSize = 12.0f;
		static constexpr float BallSpeedDefault = 900.0f;
		static constexpr float BallSpeedIncrement = 50.0f;

		static constexpr float MaxDeflectDegrees = 60.0f;
		static constexpr float MinCenterDegrees = 10.0f;

		static constexpr float Player1Speed = 900.0f;
		static constexpr float Player2Speed = 700.0f;

		static constexpr int   NumMidLines = 26;
		static constexpr float ScoreTextOffset = 75.0f;
	};
}
