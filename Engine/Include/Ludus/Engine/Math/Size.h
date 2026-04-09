#pragma once

#include <concepts>

namespace Ludus::Engine::Math
{
	template <typename T>
	concept NumericType = std::integral<T> || std::floating_point<T>;

	template<typename T>
		requires NumericType<T>
	struct Size
	{
		T Width;
		T Height;

		bool operator==(const Size<T>& other) const
		{
			return Width == other.Width && Height == other.Height;
		}
	};
}
