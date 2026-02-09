#pragma once

#include <string_view>

#include <Ludus/Engine/Persistance/Serialization/DomDocument.h>
#include <Ludus/Engine/Persistance/Serialization/DomNode.h>

namespace Ludus::Tests::Persistance::Serialization
{
	using Ludus::Engine::Persistance::Serialization::AsArray;
	using Ludus::Engine::Persistance::Serialization::AsObject;
	using Ludus::Engine::Persistance::Serialization::AsValue;

	using Ludus::Engine::Persistance::Serialization::DomArray;
	using Ludus::Engine::Persistance::Serialization::DomDocument;
	using Ludus::Engine::Persistance::Serialization::DomNode;
	using Ludus::Engine::Persistance::Serialization::DomObject;
	using Ludus::Engine::Persistance::Serialization::DomValue;

	inline const DomNode* Root(const DomDocument& document)
	{
		return document.GetRoot();
	}

	inline const DomObject& Object(const DomNode& node)
	{
		return AsObject(node);
	}

	inline const DomArray& Array(const DomNode& node)
	{
		return AsArray(node);
	}

	inline const DomValue& Value(const DomNode& node)
	{
		return AsValue(node);
	}

	inline std::string_view ObjectKeyAt(const DomNode& node, size_t index)
	{
		return AsObject(node)[index].first;
	}

	inline const DomNode* ObjectValueAt(const DomNode& node, size_t index)
	{
		return AsObject(node)[index].second.get();
	}

	inline const DomNode* ArrayValueAt(const DomNode& node, size_t index)
	{
		return AsArray(node)[index].get();
	}
}
