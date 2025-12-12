#pragma once

#include <cstdint>

namespace Ludus::Pong::Core::Configuration
{
	struct Defaults
	{
		static constexpr float PaddleWidth = 0.25f;
		static constexpr float PaddleHeight = 2.0f;
		static constexpr float PaddleXOffset = 1.25f;
		static constexpr float WallWidthThickness = 0.5f;
		static constexpr float WallHeightThickness = 0.25f;

		static constexpr float BallSize = 0.3f;
		static constexpr float BallSpeedDefault = 25.0f;
		static constexpr float BallSpeedIncrement = 0.5f;

		static constexpr float MaxDeflectDegrees = 0.06f;
		static constexpr float MinCenterDegrees = 0.01f;

		static constexpr float Player1Speed = 20.0f;
		static constexpr float Player2Speed = 18.0f;

		static constexpr int   NumMidLines = 26;
		static constexpr float ScoreTextOffset = 2.0f;
	};
}
