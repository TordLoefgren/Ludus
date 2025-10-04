#pragma once

using GameObjectHandle = uint32_t;

struct GameObject
{
private:
	inline static GameObjectHandle s_NextHandle = 1;

public:
	const GameObjectHandle Handle;

	GameObject()
		: Handle(s_NextHandle++)
	{ }
};

