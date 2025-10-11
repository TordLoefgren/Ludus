#pragma once

#include <algorithm>

namespace Ludus::Engine
{
	struct Cooldown
	{
		float Period = 0.25f;
		float Time = 0.f;

		bool CanFire() const { return Time >= Period; }
		void Consume() { Time = 0.f; }
		void Step(float deltaTime) { Time = std::min(Period, Time + deltaTime); }
	};
}
