#pragma once

#include <string_view>

#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>

namespace Ludus::EngineTests::Serialization::Core
{
	using Ludus::Engine::Serialization::Core::AsArray;
	using Ludus::Engine::Serialization::Core::AsObject;
	using Ludus::Engine::Serialization::Core::AsValue;

	using Ludus::Engine::Serialization::Core::DomArray;
	using Ludus::Engine::Serialization::Core::DomDocument;
	using Ludus::Engine::Serialization::Core::DomNode;
	using Ludus::Engine::Serialization::Core::DomObject;
	using Ludus::Engine::Serialization::Core::DomValue;

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
