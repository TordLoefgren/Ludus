#pragma once

#include <optional>
#include <vector>

#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Engine::Serialization::Core
{
	struct DomTokenStreamReader final : public ITokenStreamReader
	{
	private:
		enum class EmitState
		{
			ArrayEnd, ArrayStart, Element, Key, ObjectEnd, ObjectStart, Value,
		};

		struct NodeCursor
		{
			const DomNode* Node;
			int Index;
			EmitState State;
		};

		DomDocument& m_Document;

		std::vector<NodeCursor> m_NodeStack;
		mutable std::optional<Token> m_CurrentToken;
		bool m_IsComplete = false;

		NodeCursor GetCursor(const DomNode& node) const;
		static const DomNode* GetObjectChild(const DomNode& node, int index);
		static const std::string& GetObjectKey(const DomNode& node, int index);
		static const DomNode* GetArrayChild(const DomNode& node, int index);
		static Token MakeValueToken(const DomValue& value);
		static Token MakeTokenForNode(const DomNode& node);
		static NodeCursor CursorAfterStart(const DomNode& node);

	public:
		DomTokenStreamReader(DomDocument& document);
		virtual bool IsComplete() const override;
		virtual const Token& Peek() const override;
		virtual void Consume() override;
	};
}
