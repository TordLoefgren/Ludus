#pragma once

#include <type_traits>
#include <utility>
#include <variant>

namespace Ludus::Engine::Core::Variants
{
	template<typename... Ts>
	struct Overloaded : Ts...
	{
		using Ts::operator()...;
	};

	template<typename... Ts>
	Overloaded(Ts...) -> Overloaded<Ts...>;

	template<typename TVariant, class... TVisitors>
	void VisitOverloaded(TVariant&& variant, TVisitors&&... visitors)
	{
		(void)std::visit(
			Overloaded { std::forward<TVisitors>(visitors)... },
			std::forward<TVariant>(variant)
		);
	}

	template<typename>
	inline constexpr bool always_false_v = false;

	// Variant guard at compile time for exhaustive matching. See: 
	// https://stackoverflow.com/questions/14637356/static-assert-fails-compilation-even-though-template-function-is-called-nowhere/14637534#14637534
	template<typename T>
	constexpr void Unhandled(T&&)
	{
		static_assert(always_false_v<std::decay_t<T>>, "Unhandled variant alternative.");
	}
}
