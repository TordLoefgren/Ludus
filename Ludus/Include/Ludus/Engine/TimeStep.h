#pragma once

#include <algorithm>
#include <chrono>

namespace Ludus::Engine
{
	using Clock = std::chrono::steady_clock;

	struct TimeStep
	{
	private:
		float m_DeltaTime { 0 };
		std::chrono::time_point<Clock> m_CurrentTime;

	public:

		operator float() { return m_DeltaTime; }

		float GetSeconds() const
		{
			return m_DeltaTime;
		}

		float GetMilliseconds() const
		{
			return m_DeltaTime * 1000;
		}

		void Step()
		{
			auto newTime = Clock::now();

			// If this is the first time Step is called, we make sure we have a clean starting time.
			if (m_CurrentTime.time_since_epoch().count() == 0)
			{
				m_CurrentTime = newTime;
				m_DeltaTime = 0.0f;
				return;
			}

			std::chrono::duration<float> delta = newTime - m_CurrentTime;

			m_CurrentTime = newTime;
			m_DeltaTime = delta.count();
		}
	};
}
