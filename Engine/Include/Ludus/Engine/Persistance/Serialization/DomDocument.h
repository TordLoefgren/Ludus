#pragma once

#include <memory>
#include <string>

#include <Ludus/Engine/Persistance/Serialization/DomNode.h>

namespace Ludus::Engine::Persistance::Serialization
{
	struct DomDocument
	{
		DomNodePtr m_Root;

		DomNode* GetRoot() const { return m_Root.get(); }

		DomNodePtr MakeObjectNode() { return std::make_unique<DomNode>(DomNode { DomObject {} }); }
		DomNodePtr MakeArrayNode() { return std::make_unique<DomNode>(DomNode { DomArray {} }); }
		DomNodePtr MakeValueNode(DomValue value) { return std::make_unique<DomNode>(DomNode { std::move(value) }); }

		DomNode* SetRoot(DomNodePtr node)
		{
			m_Root = std::move(node);
			return m_Root.get();
		}

		DomNode* AddToObject(DomNode& parent, std::string key, DomNodePtr child)
		{
			auto& object = std::get<DomObject>(parent.NodeData);
			object.emplace_back(std::move(key), std::move(child));

			return object.back().second.get();
		}

		DomNode* AddToArray(DomNode& parent, DomNodePtr child)
		{
			auto& array = std::get<DomArray>(parent.NodeData);
			array.push_back(std::move(child));

			return array.back().get();
		}
	};
}
