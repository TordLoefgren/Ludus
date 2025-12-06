#pragma once

namespace Ludus::Engine::Core
{
	struct Timer
	{
		float Time = 0.0f;

		void Update(float deltaTime) { Time += deltaTime; }
		void Reset() { Time = 0.0f; }
		bool Reached(float seconds) const { return Time >= seconds; }
	};
}
