#pragma once

#include <cstdint>
#include <random>

namespace Ludus::Engine::Core
{
	class Random
	{
	private:
		std::mt19937_64 m_MT;

	public:
		/// <summary>
		/// A class that computes pseudo-random numbers in a fixed range. No seed and non-deterministic.
		/// </summary>
		Random();

		/// <summary>
		/// A class that computes pseudo-random numbers in a fixed range. Seeded and deterministic.
		/// </summary>
		/// <param name="seed">The seed to use to generate the pseudo-random numbers.</param>
		explicit Random(std::uint32_t seed);

		~Random() = default;

		/// <summary>
		/// Computes a random unsigned 64-bit integer in the maximum limits range.
		/// </summary>
		/// <returns>A random unsigned integer.</returns>
		uint64_t NextId();

		/// <summary>
		/// Computes a random integer in the range specified by min and max.
		/// </summary>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>A random integer.</returns>
		/// <exception cref="std::invalid_argument">Thrown if min &gt;= max.</exception>
		int NextInt(const int min, const int max);

		/// <summary>
		/// Computes a random unsigned integer in the range specified by min and max.
		/// </summary>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>A random unsigned integer.</returns>
		/// <exception cref="std::invalid_argument">Thrown if min &gt;= max.</exception>
		uint32_t NextUint32(const uint32_t min, const uint32_t max);

		/// <summary>
		/// Computes a random unsigned integer in the range specified by min and max.
		/// </summary>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>A random unsigned integer.</returns>
		/// <exception cref="std::invalid_argument">Thrown if min &gt;= max.</exception>
		uint64_t NextUint64(const uint64_t min = 1u, const uint64_t max = std::numeric_limits<uint64_t>::max());

		/// <summary>
		/// Computes a random float in the range specified by min and max.
		/// </summary>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>A random float.</returns>
		/// <exception cref="std::invalid_argument">Thrown if min &gt;= max.</exception>
		float NextFloat(const float min, const float max);

		/// <summary>
		/// Computes a random double in the range specified by min and max.
		/// </summary>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>A random double.</returns>
		/// <exception cref="std::invalid_argument">Thrown if min &gt;= max.</exception>
		double NextDouble(const double min, const double max);
	};
}
