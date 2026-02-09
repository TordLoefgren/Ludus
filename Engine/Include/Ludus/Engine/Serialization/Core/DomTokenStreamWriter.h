#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Engine::Serialization::Core
{
	struct DomTokenStreamWriter final : public ITokenStreamWriter
	{
	private:
		DomDocument& m_Document;
		std::vector<DomNode*> m_NodeStack;
		std::optional<std::string> m_PendingKey;

		DomNode* AttachNode(DomNodePtr node)
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
			LUDUS_ASSERT(m_PendingKey.has_value(), "Object child requires a key.");

			std::string key = std::move(*m_PendingKey);
			m_PendingKey.reset();
			return m_Document.AddToObject(*parent, std::move(key), std::move(node));
		}

	public:
		DomTokenStreamWriter(DomDocument& document)
			: m_Document(document)
		{ }

		virtual void Emit(const Token& token) override
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
						LUDUS_ASSERT(!m_PendingKey.has_value(), "EndObject cannot close with a pending key.");
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
						LUDUS_ASSERT(!m_PendingKey.has_value(), "Cannot emit a key without consuming the previous key.");
						m_PendingKey = std::string(value.Data);
					},
				[&](const Token::Null&) { AttachNode(m_Document.MakeValueNode(std::monostate{})); },

				[&](const Token::Bool& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
				[&](const Token::Double& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
				[&](const Token::Float& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
				[&](const Token::Int& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
				[&](const Token::String& value) { AttachNode(m_Document.MakeValueNode(std::string(value.Data))); },
				[&](const Token::Uint32& value) { AttachNode(m_Document.MakeValueNode(value.Data)); },
				[&](auto&&) { LUDUS_ASSERT(false, "Unhandled token type."); },
				}, token.Data);
		}
	};
}
