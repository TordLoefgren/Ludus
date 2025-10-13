#pragma once

#include <algorithm>

namespace Ludus::Engine
{
	struct Cooldown
	{
		float Duration = 0.25f;
		float Time = 0.0f;

		bool IsElapsed() const { return Time >= Duration; }
		void Reset() { Time = 0.0f; }
		void Step(float deltaTime) { Time = std::min(Duration, Time + deltaTime); }
	};
}
