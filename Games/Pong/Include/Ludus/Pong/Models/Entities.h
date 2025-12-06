#pragma once

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Pong::Models
{
	struct Entities
	{
		Ludus::Engine::Core::EntityHandle Player1Handle = 0;
		Ludus::Engine::Core::EntityHandle Player2Handle = 0;
		Ludus::Engine::Core::EntityHandle BallHandle = 0;
		Ludus::Engine::Core::EntityHandle LeftWallHandle = 0;
		Ludus::Engine::Core::EntityHandle TopWallHandle = 0;
		Ludus::Engine::Core::EntityHandle RightWallHandle = 0;
		Ludus::Engine::Core::EntityHandle BottomWallHandle = 0;
		Ludus::Engine::Core::EntityHandle LeftScoreTextHandle = 0;
		Ludus::Engine::Core::EntityHandle RightScoreTextHandle = 0;
		std::vector<Ludus::Engine::Core::EntityHandle> CenterLineHandles;
	};
}
