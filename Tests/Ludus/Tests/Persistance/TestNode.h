#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace Ludus::Tests::Persistance
{
	struct TestNode
	{
		enum class Kind { Object, Array, Value };

		Kind NodeKind = Kind::Object;

		std::unordered_map<std::string, TestNode> Object;
		std::vector<TestNode> Array;
		std::variant<bool, double, float, int, std::string> Value;

		static TestNode MakeObject() { return { Kind::Object }; }
		static TestNode MakeArray() { return { Kind::Array }; }

		template <class T>
		static constexpr bool IsSupportedValue =
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int> ||
			std::is_same_v<std::decay_t<T>, float> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, std::string>;

		template<typename T>
		static TestNode MakeValue(T value)
			requires (IsSupportedValue<T>)
		{
			TestNode node;
			node.NodeKind = Kind::Value;
			node.Value = std::move(value);

			return node;
		}

		static TestNode MakeValue(std::string_view value)
		{
			return MakeValue(std::string(value));
		}

		static TestNode MakeValue(const char* value)
		{
			return MakeValue(std::string(value));
		}
	};
}
