#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

namespace Ludus::Engine::Serialization::Core
{
	template<class... Ts>
	struct Overloaded : Ts... { using Ts::operator()...; };

	template<class... Ts>
	Overloaded(Ts...) -> Overloaded<Ts...>;

	struct Token
	{
		struct StartObject { };
		struct EndObject { };

		struct StartArray { };
		struct EndArray { };

		struct Key { std::string_view Data; };
		struct Null { };

		struct Bool { bool Data; };
		struct Int { int Data; };
		struct Uint32 { uint32_t Data; };
		struct Float { float Data; };
		struct Double { double Data; };
		struct String { std::string_view Data; };

		using Variant = std::variant<
			StartObject, EndObject,
			StartArray, EndArray,
			Key, Null,
			Bool, Int, Uint32, Float, Double, String
		>;

		Variant Data;

		template<class T>
		Token(T value) : Data(std::move(value)) { }
	};
}
