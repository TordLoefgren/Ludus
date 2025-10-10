#pragma once

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
			std::chrono::duration<double> delta = newTime - m_CurrentTime;
			auto deltaTime = delta.count();

			m_CurrentTime = newTime;
			m_DeltaTime = static_cast<float>(deltaTime);
		}
	};
}
