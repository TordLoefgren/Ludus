#include <Ludus/Engine/Random.h>

#include <stdexcept>

namespace Ludus::Engine
{
	Random::Random()
		: m_MT(std::random_device { }())
	{ }

	Random::Random(std::uint32_t seed)
		: m_MT(seed)
	{ }

	int Random::NextInt(const int min, const int max)
	{
		if (min >= max)
		{
			throw std::invalid_argument("The minimum value must be less than the maximum value.");
		}

		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_MT);
	}

	uint32_t Random::NextUint(const uint32_t min, const uint32_t max)
	{
		if (min >= max)
		{
			throw std::invalid_argument("The minimum value must be less than the maximum value.");
		}

		std::uniform_int_distribution<uint32_t> dist(min, max);
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
