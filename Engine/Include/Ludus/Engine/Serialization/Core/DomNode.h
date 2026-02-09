#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Serialization::Core
{
	struct DomNode;

	using DomNodePtr = std::unique_ptr<DomNode>;
	using DomArray = std::vector<DomNodePtr>;
	using DomObject = std::vector<std::pair<std::string, DomNodePtr>>;
	using DomValue = std::variant<std::monostate, bool, double, float, int, std::string, uint8_t, uint32_t>;

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

	inline const DomArray& AsArray(const DomNode& node)
	{
		LUDUS_ASSERT(std::holds_alternative<DomArray>(node.NodeData), "Node is not an array.");
		return std::get<DomArray>(node.NodeData);
	}

	inline const DomObject& AsObject(const DomNode& node)
	{
		LUDUS_ASSERT(std::holds_alternative<DomObject>(node.NodeData), "Node is not an object.");
		return std::get<DomObject>(node.NodeData);
	}

	inline const DomValue& AsValue(const DomNode& node)
	{
		LUDUS_ASSERT(std::holds_alternative<DomValue>(node.NodeData), "Node is not a value.");
		return std::get<DomValue>(node.NodeData);
	}

	inline bool IsArray(const DomNode& node) { return std::holds_alternative<DomArray>(node.NodeData); }
	inline bool IsObject(const DomNode& node) { return std::holds_alternative<DomObject>(node.NodeData); }
	inline bool IsValue(const DomNode& node) { return std::holds_alternative<DomValue>(node.NodeData); }
}
