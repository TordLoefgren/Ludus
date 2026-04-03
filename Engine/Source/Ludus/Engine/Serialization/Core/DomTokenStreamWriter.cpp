#include "pch.h"

#include <string>
#include <utility>
#include <variant>

#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>

namespace Ludus::Engine::Serialization::Core
{
	DomNode* DomTokenStreamWriter::AttachNode(DomNodePtr node)
	{
		if (m_NodeStack.empty())
		{
			LUDUS_ASSERT(m_Document.GetRoot() == nullptr, "Root node already exists.");
			return m_Document.SetRoot(std::move(node));
		}

		auto* parent = m_NodeStack.back();
		if (std::holds_alternative<DomArray>(parent->NodeData))
		{
			return m_Document.AddToArray(*parent, std::move(node));
		}

		LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "Parent must be an object or array.");
		LUDUS_ASSERT(m_PendingKey, "Object child requires a key.");

		std::string key = std::move(*m_PendingKey);
		m_PendingKey.reset();
		return m_Document.AddToObject(*parent, std::move(key), std::move(node));
	}

	DomTokenStreamWriter::DomTokenStreamWriter(DomDocument& document)
		: m_Document(document)
	{ }

	void DomTokenStreamWriter::Emit(const Token& token)
	{
		std::visit(Overloaded
			{
				[&](const Token::StartObject&)
		{
			auto* node = AttachNode(m_Document.MakeObjectNode());
			m_NodeStack.push_back(node);
		},
			[&](const Token::EndObject&)
		{
			LUDUS_ASSERT(!m_NodeStack.empty(), "EndObject requires an active object.");
			LUDUS_ASSERT(std::holds_alternative<DomObject>(m_NodeStack.back()->NodeData), "EndObject must close an object.");
			LUDUS_ASSERT(!m_PendingKey, "EndObject cannot close with a pending key.");
			m_NodeStack.pop_back();
		},

			[&](const Token::StartArray&)
		{
			auto* node = AttachNode(m_Document.MakeArrayNode());
			m_NodeStack.push_back(node);
		},
			[&](const Token::EndArray&)
		{
			LUDUS_ASSERT(!m_NodeStack.empty(), "EndArray requires an active array.");
			LUDUS_ASSERT(std::holds_alternative<DomArray>(m_NodeStack.back()->NodeData), "EndArray must close an array.");
			m_NodeStack.pop_back();
		},

			[&](const Token::Key& value)
		{
			LUDUS_ASSERT(!m_NodeStack.empty(), "Key requires an active object.");
			LUDUS_ASSERT(std::holds_alternative<DomObject>(m_NodeStack.back()->NodeData), "Key must be used inside an object.");
			LUDUS_ASSERT(!m_PendingKey, "Cannot emit a key without consuming the previous key.");
			m_PendingKey = std::string(value.Data);
		},
			[&](const Token::Null&) { AttachNode(m_Document.MakeValueNode(std::monostate { })); },

			[&](const Token::Bool& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
			[&](const Token::Double& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
			[&](const Token::Int& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
			[&](const Token::String& value) { AttachNode(m_Document.MakeValueNode(std::string(value.Data))); },
			[&](const Token::Uint& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
			[&](auto&&) { LUDUS_ASSERT(false, "Unhandled token type."); },
			}, token.Data);
	}
}
