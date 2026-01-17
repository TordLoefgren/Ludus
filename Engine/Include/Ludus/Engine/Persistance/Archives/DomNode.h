#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace Ludus::Engine::Persistance::Archives
{
	struct DomNode;

	using DomObject = std::unordered_map <std::string, DomNode>;

	using DomArray = std::vector<DomNode>;

	using DomValue = std::variant<bool, double, float, int, std::string, uint8_t, uint32_t>;

	struct DomNode
	{
		using Data = std::variant<DomObject, DomArray, DomValue>;
		Data NodeData;
	};

	template <class T>
	static constexpr bool IsSupportedValue =
		std::is_same_v<std::decay_t<T>, bool> ||
		std::is_same_v<std::decay_t<T>, int> ||
		std::is_same_v<std::decay_t<T>, float> ||
		std::is_same_v<std::decay_t<T>, double> ||
		std::is_same_v<std::decay_t<T>, std::string> ||
		std::is_same_v<std::decay_t<T>, uint8_t> ||
		std::is_same_v<std::decay_t<T>, uint32_t>;

	inline DomNode MakeObjectNode() { return DomNode { DomObject{} }; }

	inline DomNode MakeArrayNode() { return DomNode { DomArray{} }; }

	template<class T>
		requires (IsSupportedValue<T>)
	inline DomNode MakeValueNode(T v) { return DomNode { DomValue{std::move(v)} }; }

	inline DomNode MakeValueNode(std::string_view value) { return MakeValueNode(std::string(value)); }

	inline DomNode MakeValueNode(const char* value) { return MakeValueNode(std::string(value)); }

	inline const DomObject& AsObject(const DomNode& node)
	{
		LUDUS_ASSERT(std::holds_alternative<DomObject>(node.NodeData), "Node is not an object.");
		return std::get<DomObject>(node.NodeData);
	}

	inline const DomArray& AsArray(const DomNode& node)
	{
		LUDUS_ASSERT(std::holds_alternative<DomArray>(node.NodeData), "Node is not an array.");
		return std::get<DomArray>(node.NodeData);
	}

	inline const DomValue& AsValue(const DomNode& node)
	{
		LUDUS_ASSERT(std::holds_alternative<DomValue>(node.NodeData), "Node is not a value.");
		return std::get<DomValue>(node.NodeData);
	}
}
