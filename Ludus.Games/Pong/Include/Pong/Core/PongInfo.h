#pragma once

namespace Pong::Core
{
	struct PongInfo
	{
		int Player1Score = 0;
		int Player2Score = 0;
		int MaxScore = 5;
		bool IsMultiplayer = false;
	};
}
