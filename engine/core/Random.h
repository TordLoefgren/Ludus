#pragma once

#include <random>

class Random
{
private:
	std::random_device m_RandomDevice;
	std::mt19937 m_MT;

public:
	Random();
	~Random();

	float Next(const float min, const float max);

	//template<typename T> T Next(const T min, const T max);
};

//template<typename T>
//inline T Random::Next(const T min, const T max)
//{
//	return T();
//}



