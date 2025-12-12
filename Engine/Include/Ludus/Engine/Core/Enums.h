#pragma once

#include <assert.h>

namespace Ludus::Engine::Core::Enums
{
	// Trait for enumerating values.
	template<typename TEnum>
	struct GetValues
	{
		static_assert(sizeof(TEnum) == 0, "GetValues<TEnum> is not specialized for this enum type.");
	};

	// Concept ensuring that values exist for enumeration.
	template<typename TEnum>
	concept HasValues = requires {
		GetValues<TEnum>::Values;
	};
}
