#pragma once

#include <random>

namespace Ludus::Engine
{
	class Random
	{
	private:
		std::random_device m_RandomDevice;
		std::mt19937 m_MT;

	public:
		Random();
		~Random();

		float Next(const float min, const float max);
	};
}
