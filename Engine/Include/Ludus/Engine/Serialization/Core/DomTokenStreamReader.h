#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Engine::Serialization::Core
{
	struct DomTokenStreamReader final : public ITokenStreamReader
	{
	private:
		enum EmitState
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

		NodeCursor GetCursor(const DomNode& node) const
		{
			int index = -1;
			EmitState state;

			if (IsArray(node))
			{
				state = EmitState::ArrayStart;
			}
			else if (IsObject(node))
			{
				state = EmitState::ObjectStart;
			}
			else
			{
				state = EmitState::Value;
			}

			return { &node, index, state };
		}

		static const DomNode* GetObjectChild(const DomNode& node, int index)
		{
			const auto& object = AsObject(node);
			LUDUS_ASSERT(index >= 0 && index < static_cast<int>(object.size()), "Object index out of range.");
			return object[static_cast<size_t>(index)].second.get();
		}

		static const std::string& GetObjectKey(const DomNode& node, int index)
		{
			const auto& object = AsObject(node);
			LUDUS_ASSERT(index >= 0 && index < static_cast<int>(object.size()), "Object index out of range.");
			return object[static_cast<size_t>(index)].first;
		}

		static const DomNode* GetArrayChild(const DomNode& node, int index)
		{
			const auto& array = AsArray(node);
			LUDUS_ASSERT(index >= 0 && index < static_cast<int>(array.size()), "Array index out of range.");
			return array[static_cast<size_t>(index)].get();
		}

		static Token MakeValueToken(const DomValue& value)
		{
			return std::visit(Overloaded
				{
					[](std::monostate) { return Token(Token::Null {}); },
					[](bool data) { return Token(Token::Bool { data }); },
					[](double data) { return Token(Token::Double { data }); },
					[](int64_t data) { return Token(Token::Int { data }); },
					[](const std::string& data) { return Token(Token::String { data }); },
					[](uint64_t data) { return Token(Token::Uint { data }); },
				}, value);
		}

		static Token MakeTokenForNode(const DomNode& node)
		{
			if (IsObject(node))
			{
				return Token(Token::StartObject {});
			}

			if (IsArray(node))
			{
				return Token(Token::StartArray {});
			}

			return MakeValueToken(AsValue(node));
		}

		static NodeCursor CursorAfterStart(const DomNode& node)
		{
			NodeCursor cursor { &node, -1, EmitState::Value };

			if (IsObject(node))
			{
				cursor.State = AsObject(node).empty() ? EmitState::ObjectEnd : EmitState::Key;
				cursor.Index = AsObject(node).empty() ? -1 : 0;
				return cursor;
			}

			if (IsArray(node))
			{
				cursor.State = AsArray(node).empty() ? EmitState::ArrayEnd : EmitState::Element;
				cursor.Index = AsArray(node).empty() ? -1 : 0;
				return cursor;
			}

			cursor.State = EmitState::Value;
			return cursor;
		}

	public:
		DomTokenStreamReader(DomDocument& document)
			: m_Document(document)
		{
			const auto* root = m_Document.GetRoot();
			LUDUS_ASSERT(root != nullptr, "The DOM document is empty.");

			if (root)
			{
				auto nodeCursor = GetCursor(*root);
				m_NodeStack.push_back(nodeCursor);
			}
		}

		virtual bool IsComplete() const override
		{
			return m_IsComplete;
		}

		virtual const Token& Peek() const override
		{
			LUDUS_ASSERT(!m_IsComplete, "Peek cannot be called on a completed DOM document.");
			LUDUS_ASSERT(!m_NodeStack.empty(), "The DOM document does not have a valid token.");

			if (m_CurrentToken.has_value())
			{
				return *m_CurrentToken;
			}

			const auto& cursor = m_NodeStack.back();
			switch (cursor.State)
			{
				case EmitState::ArrayEnd:
				{
					m_CurrentToken = Token::EndArray {};
					break;
				}
				case EmitState::ArrayStart:
				{
					m_CurrentToken = Token::StartArray {};
					break;
				}
				case EmitState::Element:
				{
					const auto* child = GetArrayChild(*cursor.Node, cursor.Index);
					m_CurrentToken = MakeTokenForNode(*child);
					break;
				}
				case EmitState::Key:
				{
					m_CurrentToken = Token::Key { GetObjectKey(*cursor.Node, cursor.Index) };
					break;
				}
				case EmitState::ObjectEnd:
				{
					m_CurrentToken = Token::EndObject {};
					break;
				}
				case EmitState::ObjectStart:
				{
					m_CurrentToken = Token::StartObject {};
					break;
				}
				case EmitState::Value:
				{
					if (IsObject(*cursor.Node))
					{
						const auto* child = GetObjectChild(*cursor.Node, cursor.Index);
						m_CurrentToken = MakeTokenForNode(*child);
						break;
					}

					m_CurrentToken = MakeTokenForNode(*cursor.Node);
					break;
				}
				default:
					throw SerializationException("Invalid EmitState type.");
			}

			return *m_CurrentToken;
		}

		virtual void Consume() override
		{
			if (m_IsComplete)
			{
				return;
			}

			if (!m_CurrentToken.has_value())
			{
				(void)Peek();
			}

			auto& cursor = m_NodeStack.back();
			switch (cursor.State)
			{
				case EmitState::ArrayEnd:
				{
					m_NodeStack.pop_back();
					break;
				}
				case EmitState::ArrayStart:
				{
					const auto& array = AsArray(*cursor.Node);
					if (array.empty())
					{
						cursor.State = EmitState::ArrayEnd;
						break;
					}

					cursor.Index = 0;
					cursor.State = EmitState::Element;
					break;
				}
				case EmitState::Element:
				{
					const auto& array = AsArray(*cursor.Node);
					const auto* child = GetArrayChild(*cursor.Node, cursor.Index);

					if (static_cast<size_t>(cursor.Index + 1) >= array.size())
					{
						cursor.State = EmitState::ArrayEnd;
					}
					else
					{
						cursor.Index += 1;
						cursor.State = EmitState::Element;
					}

					if (IsObject(*child) || IsArray(*child))
					{
						m_NodeStack.push_back(CursorAfterStart(*child));
					}
					break;
				}
				case EmitState::Key:
				{
					cursor.State = EmitState::Value;
					break;
				}
				case EmitState::ObjectEnd:
				{
					m_NodeStack.pop_back();
					break;
				}
				case EmitState::ObjectStart:
				{
					const auto& object = AsObject(*cursor.Node);
					if (object.empty())
					{
						cursor.State = EmitState::ObjectEnd;
						break;
					}

					cursor.Index = 0;
					cursor.State = EmitState::Key;
					break;
				}
				case EmitState::Value:
				{
					if (IsObject(*cursor.Node))
					{
						const auto& object = AsObject(*cursor.Node);
						const auto* child = GetObjectChild(*cursor.Node, cursor.Index);

						if (static_cast<size_t>(cursor.Index + 1) >= object.size())
						{
							cursor.State = EmitState::ObjectEnd;
						}
						else
						{
							cursor.Index += 1;
							cursor.State = EmitState::Key;
						}

						if (IsObject(*child) || IsArray(*child))
						{
							m_NodeStack.push_back(CursorAfterStart(*child));
						}

						break;
					}

					m_NodeStack.pop_back();
					break;
				}
				default:
					throw SerializationException("Invalid EmitState type.");
			}

			m_CurrentToken.reset();

			if (m_NodeStack.empty() && !m_CurrentToken.has_value())
			{
				m_IsComplete = true;
			}
		}
	};
}
