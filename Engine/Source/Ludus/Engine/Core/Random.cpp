#include "pch.h"

#include <cstdint>
#include <random>
#include <stdexcept>

#include <Ludus/Engine/Core/Random.h>

namespace Ludus::Engine::Core
{
	Random::Random()
		: m_MT(std::random_device { }())
	{ }

	Random::Random(std::uint32_t seed)
		: m_MT(std::mt19937_64 { seed })
	{ }

	uint64_t Random::NextId()
	{
		return NextUint64(1u, std::numeric_limits<uint64_t>::max());
	}

	int Random::NextInt(const int min, const int max)
	{
		if (min >= max)
		{
			throw std::invalid_argument("The minimum value must be less than the maximum value.");
		}

		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_MT);
	}

	uint32_t Random::NextUint32(const uint32_t min, const uint32_t max)
	{
		if (min >= max)
		{
			throw std::invalid_argument("The minimum value must be less than the maximum value.");
		}

		std::uniform_int_distribution<uint32_t> dist(min, max);
		return dist(m_MT);
	}

	uint64_t Random::NextUint64(uint64_t min, uint64_t max)
	{
		if (min >= max)
			throw std::invalid_argument("The minimum value must be less than the maximum value.");

		std::uniform_int_distribution<uint64_t> dist(min, max);
		return dist(m_MT);
	}

	float Random::NextFloat(const float min, const float max)
	{
		if (min >= max)
		{
			throw std::invalid_argument("The minimum value must be less than the maximum value.");
		}

		std::uniform_real_distribution<float> dist(min, max);
		return dist(m_MT);
	}

	double Random::NextDouble(const double min, const double max)
	{
		if (min >= max)
		{
			throw std::invalid_argument("The minimum value must be less than the maximum value.");
		}

		std::uniform_real_distribution<double> dist(min, max);
		return dist(m_MT);
	}
}
