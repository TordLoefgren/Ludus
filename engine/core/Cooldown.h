#pragma once

#include <algorithm>

struct Cooldown
{
	float period = 0.25f;  // seconds
	float time = 0.f;


	bool CanFire() const { return time >= period; }
	void Consume() { time = 0.f; }
	void Step(float deltaTime) { time = std::min(period, time + deltaTime); }
};
