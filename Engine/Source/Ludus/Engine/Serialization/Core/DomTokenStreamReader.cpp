#include "pch.h"

#include <cstdint>
#include <string>
#include <variant>

#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>

namespace Ludus::Engine::Serialization::Core
{
	DomTokenStreamReader::NodeCursor DomTokenStreamReader::GetCursor(const DomNode& node) const
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

	const DomNode* DomTokenStreamReader::GetObjectChild(const DomNode& node, int index)
	{
		const auto& object = AsObject(node);
		LUDUS_ASSERT(index >= 0 && index < static_cast<int>(object.size()), "Object index out of range.");
		return object[static_cast<size_t>(index)].second.get();
	}

	const std::string& DomTokenStreamReader::GetObjectKey(const DomNode& node, int index)
	{
		const auto& object = AsObject(node);
		LUDUS_ASSERT(index >= 0 && index < static_cast<int>(object.size()), "Object index out of range.");
		return object[static_cast<size_t>(index)].first;
	}

	const DomNode* DomTokenStreamReader::GetArrayChild(const DomNode& node, int index)
	{
		const auto& array = AsArray(node);
		LUDUS_ASSERT(index >= 0 && index < static_cast<int>(array.size()), "Array index out of range.");
		return array[static_cast<size_t>(index)].get();
	}

	Token DomTokenStreamReader::MakeValueToken(const DomValue& value)
	{
		return std::visit(Overloaded
			{
				[](std::monostate) { return Token(Token::Null { }); },
				[](bool data) { return Token(Token::Bool { data }); },
				[](double data) { return Token(Token::Double { data }); },
				[](int64_t data) { return Token(Token::Int { data }); },
				[](const std::string& data) { return Token(Token::String { data }); },
				[](uint64_t data) { return Token(Token::Uint { data }); },
			}, value);
	}

	Token DomTokenStreamReader::MakeTokenForNode(const DomNode& node)
	{
		if (IsObject(node))
		{
			return Token(Token::StartObject { });
		}

		if (IsArray(node))
		{
			return Token(Token::StartArray { });
		}

		return MakeValueToken(AsValue(node));
	}

	DomTokenStreamReader::NodeCursor DomTokenStreamReader::CursorAfterStart(const DomNode& node)
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

	DomTokenStreamReader::DomTokenStreamReader(DomDocument& document)
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

	bool DomTokenStreamReader::IsComplete() const
	{
		return m_IsComplete;
	}

	const Token& DomTokenStreamReader::Peek() const
	{
		LUDUS_ASSERT(!m_IsComplete, "Peek cannot be called on a completed DOM document.");
		LUDUS_ASSERT(!m_NodeStack.empty(), "The DOM document does not have a valid token.");

		if (m_CurrentToken)
		{
			return *m_CurrentToken;
		}

		const auto& cursor = m_NodeStack.back();
		switch (cursor.State)
		{
			case EmitState::ArrayEnd:
			{
				m_CurrentToken = Token::EndArray { };
				break;
			}
			case EmitState::ArrayStart:
			{
				m_CurrentToken = Token::StartArray { };
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
				m_CurrentToken = Token::EndObject { };
				break;
			}
			case EmitState::ObjectStart:
			{
				m_CurrentToken = Token::StartObject { };
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

	void DomTokenStreamReader::Consume()
	{
		if (m_IsComplete)
		{
			return;
		}

		if (!m_CurrentToken)
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

		if (m_NodeStack.empty() && !m_CurrentToken)
		{
			m_IsComplete = true;
		}
	}
}
