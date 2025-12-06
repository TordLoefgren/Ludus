#pragma once

#include <algorithm>
#include <chrono>

namespace Ludus::Engine::Core
{
	inline constexpr float FIXED_DELTA_TIME = 1.0f / 60.0f;
	inline constexpr float MAX_FRAME_TIME = 0.20f;

	using Clock = std::chrono::steady_clock;

	struct Time
	{
	private:
		float m_DeltaTime { 0 };
		float m_Accumulator { 0 };
		std::chrono::time_point<Clock> m_CurrentTime { };
		bool m_IsRunning { false };

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

		float GetFixed() const
		{
			return FIXED_DELTA_TIME;
		}

		void Step()
		{
			auto now = Clock::now();

			if (!m_IsRunning)
			{
				m_CurrentTime = now;
				m_DeltaTime = 0.0f;
				m_IsRunning = true;
				return;
			}

			std::chrono::duration<float> delta = now - m_CurrentTime;
			m_CurrentTime = now;

			// Clamp frame delta time to cap accumulator growth.
			m_DeltaTime = std::min(delta.count(), MAX_FRAME_TIME);
			m_Accumulator += m_DeltaTime;
		}

		bool ConsumeFixed()
		{
			if (m_Accumulator >= FIXED_DELTA_TIME)
			{
				m_Accumulator -= FIXED_DELTA_TIME;
				return true;
			}

			return false;
		}
	};
}
