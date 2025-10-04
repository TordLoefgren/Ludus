#pragma once

struct Timer
{
	float time = 0.f;

	void Update(float dt) { time += dt; }
	void Reset() { time = 0.f; }
	bool Reached(float seconds) const { return time >= seconds; }
};
