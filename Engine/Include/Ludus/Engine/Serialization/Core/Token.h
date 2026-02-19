#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

#include <Ludus/Engine/Core/Variants.h>

namespace Ludus::Engine::Serialization::Core
{
	using Ludus::Engine::Core::Variants::Overloaded;

	struct Token
	{
		struct StartObject { };
		struct EndObject { };

		struct StartArray { };
		struct EndArray { };

		struct Key { std::string_view Data; };
		struct Null { };

		struct Bool { bool Data; };
		struct Int { int64_t Data; };
		struct Uint { uint64_t Data; };
		struct Double { double Data; };
		struct String { std::string_view Data; };

		using Variant = std::variant<
			StartObject, EndObject,
			StartArray, EndArray,
			Key, Null,
			Bool, Int, Uint, Double, String
		>;

		Variant Data;

		template<class T>
		Token(T value) : Data(std::move(value)) { }
	};
}
