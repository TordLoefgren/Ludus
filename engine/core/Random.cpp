#include "Random.h"

Random::Random()
	: m_RandomDevice(), m_MT(m_RandomDevice())
{ }

Random::~Random()
{ }

float Random::Next(const float min, const float max)
{
	// See: https://stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_MT);
}