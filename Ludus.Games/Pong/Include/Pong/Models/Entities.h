#pragma once

#include <Ludus/Engine/GameObject.h>

namespace Pong::Models
{
	struct Entities
	{
		Ludus::Engine::GameObjectHandle Player1Handle = 0;
		Ludus::Engine::GameObjectHandle Player2Handle = 0;
		Ludus::Engine::GameObjectHandle BallHandle = 0;
		Ludus::Engine::GameObjectHandle LeftWallHandle = 0;
		Ludus::Engine::GameObjectHandle TopWallHandle = 0;
		Ludus::Engine::GameObjectHandle RightWallHandle = 0;
		Ludus::Engine::GameObjectHandle BottomWallHandle = 0;
	};
}
