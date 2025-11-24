#pragma once

#include <Ludus/Engine/Entity.h>

namespace Pong::Models
{
	struct Entities
	{
		Ludus::Engine::EntityHandle Player1Handle = 0;
		Ludus::Engine::EntityHandle Player2Handle = 0;
		Ludus::Engine::EntityHandle BallHandle = 0;
		Ludus::Engine::EntityHandle LeftWallHandle = 0;
		Ludus::Engine::EntityHandle TopWallHandle = 0;
		Ludus::Engine::EntityHandle RightWallHandle = 0;
		Ludus::Engine::EntityHandle BottomWallHandle = 0;
		Ludus::Engine::EntityHandle LeftScoreTextHandle = 0;
		Ludus::Engine::EntityHandle RightScoreTextHandle = 0;
		std::vector<Ludus::Engine::EntityHandle> CenterLineHandles;
	};
}
