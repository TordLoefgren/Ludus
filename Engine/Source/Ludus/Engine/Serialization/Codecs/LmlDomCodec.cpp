#include "pch.h"

#include <cstdint>
#include <format>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

#include <Ludus/Engine/Serialization/Codecs/LmlDomCodec.h>

namespace
{
	using DomNode = Ludus::Engine::Serialization::Core::DomNode;
	using DomArray = Ludus::Engine::Serialization::Core::DomArray;
	using DomObject = Ludus::Engine::Serialization::Core::DomObject;
	using DomValue = Ludus::Engine::Serialization::Core::DomValue;

	constexpr size_t IndentWidth = 2;
	constexpr size_t InlineArrayMax = 8;
	constexpr size_t InlineObjectMaxPairs = 2;
	constexpr size_t MaxLineLength = 100;

	struct WriterContext
	{
		std::string Text;
		size_t Indentation = 0;
	};

	void AppendIndent(std::string& out, size_t spaces)
	{
		out.append(spaces, ' ');
	}

	bool IsNumberLiteralForString(std::string_view raw)
	{
		if (raw.empty())
		{
			return false;
		}

		size_t i = 0;
		if (raw[0] == '-')
		{
			if (raw.size() == 1)
			{
				return false;
			}
			i = 1;
		}

		bool sawDigit = false;
		bool sawDot = false;

		for (; i < raw.size(); i++)
		{
			const char c = raw[i];
			if (c >= '0' && c <= '9')
			{
				sawDigit = true;
				continue;
			}
			if (c == '.' && !sawDot)
			{
				sawDot = true;
				continue;
			}
			return false;
		}

		return sawDigit;
	}

	bool ShouldQuoteString(std::string_view raw)
	{
		if (raw == "null" || raw == "true" || raw == "false" || IsNumberLiteralForString(raw))
		{
			return true;
		}

		for (char c : raw)
		{
			switch (c)
			{
				case ':': case '#': case '\n': case '"': case '\\': case '-':
					return true;
				default:
					break;
			}
		}
		return false;
	}

	void AppendEscaped(std::string& out, std::string_view raw)
	{
		for (char c : raw)
		{
			switch (c)
			{
				case '\n': out += "\\n"; break;
				case '"': out += "\\\""; break;
				case '\\': out += "\\\\"; break;
				default: out.push_back(c); break;
			}
		}
	}

	void EmitScalar(std::string& out, const DomValue& value)
	{
		std::visit([&](const auto& v)
		{
			using T = std::decay_t<decltype(v)>;

			if constexpr (std::is_same_v<T, std::monostate>)
			{
				out += "null";
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				out += (v ? "true" : "false");
			}
			else if constexpr (std::is_same_v<T, double>)
			{
				out += std::format("{:.2f}", v);
			}
			else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>)
			{
				out += std::format("{}", v);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				if (ShouldQuoteString(v))
				{
					out.push_back('"');
					AppendEscaped(out, v);
					out.push_back('"');
				}
				else
				{
					out += v;
				}
			}
		}, value);
	}

	bool IsScalarNode(const DomNode* n)
	{
		return std::holds_alternative<DomValue>(n->NodeData);
	}

	bool IsEmptyArrayNode(const DomNode* n)
	{
		return std::holds_alternative<DomArray>(n->NodeData) && std::get<DomArray>(n->NodeData).empty();
	}

	bool IsEmptyObjectNode(const DomNode* n)
	{
		return std::holds_alternative<DomObject>(n->NodeData) && std::get<DomObject>(n->NodeData).empty();
	}

	bool CanInlineScalarArray(const DomArray& arr)
	{
		if (arr.empty())
		{
			return true;
		}

		if (arr.size() > InlineArrayMax)
		{
			return false;
		}

		size_t len = 2;
		for (size_t i = 0; i < arr.size(); i++)
		{
			const DomNode* child = arr.at(i).get();
			if (!IsScalarNode(child))
			{
				return false;
			}

			std::string temp;
			EmitScalar(temp, std::get<DomValue>(child->NodeData));
			len += temp.size();

			if (i + 1 < arr.size())
			{
				len += 2;
			}
			if (len > MaxLineLength)
			{
				return false;
			}
		}
		return true;
	}

	bool CanInlineScalarObject(const DomObject& obj)
	{
		if (obj.empty())
		{
			return true;
		}

		if (obj.size() > InlineObjectMaxPairs)
		{
			return false;
		}

		size_t len = 4;
		for (size_t i = 0; i < obj.size(); i++)
		{
			const auto& [key, valuePtr] = obj.at(i);
			const DomNode* value = valuePtr.get();
			if (!IsScalarNode(value))
			{
				return false;
			}

			std::string temp;
			EmitScalar(temp, std::get<DomValue>(value->NodeData));

			len += key.size();
			len += 2;
			len += temp.size();

			if (i + 1 < obj.size())
			{
				len += 2;
			}

			if (len > MaxLineLength)
			{
				return false;
			}
		}
		return true;
	}

	bool CanInlineDocumentObjectValue(const DomObject& obj)
	{
		return obj.size() == 2 && CanInlineScalarObject(obj);
	}

	void ValidateNoMixedArrays(const DomArray& arr)
	{
		bool anyScalar = false;
		bool anyObject = false;
		bool anyArray = false;

		for (const auto& el : arr)
		{
			const DomNode* n = el.get();
			anyScalar |= std::holds_alternative<DomValue>(n->NodeData);
			anyObject |= std::holds_alternative<DomObject>(n->NodeData);
			anyArray |= std::holds_alternative<DomArray>(n->NodeData);
		}

		const int kinds = (anyScalar ? 1 : 0) + (anyObject ? 1 : 0) + (anyArray ? 1 : 0);
		if (kinds > 1)
		{
			throw std::runtime_error("LmlDomCodec: Mixed arrays are not allowed.");
		}
	}

	struct Writer
	{
		WriterContext& Context;

		void WriteRoot(const DomNode* node)
		{
			if (!std::holds_alternative<DomObject>(node->NodeData))
			{
				WriteValue(node);
				return;
			}
			WriteDocumentObject(std::get<DomObject>(node->NodeData));
		}

		void WriteValue(const DomNode* node)
		{
			if (std::holds_alternative<DomValue>(node->NodeData))
			{
				EmitScalar(Context.Text, std::get<DomValue>(node->NodeData));
				return;
			}

			if (std::holds_alternative<DomArray>(node->NodeData))
			{
				WriteArrayValue(std::get<DomArray>(node->NodeData));
				return;
			}

			WriteObjectValue(std::get<DomObject>(node->NodeData));
		}

		void WriteDocumentObject(const DomObject& obj)
		{
			for (size_t i = 0; i < obj.size(); i++)
			{
				const auto& [key, valuePtr] = obj.at(i);
				const DomNode* value = valuePtr.get();

				AppendIndent(Context.Text, Context.Indentation);
				Context.Text += key;
				WriteDocumentKeyValue(value);
			}
		}

		void WriteDocumentKeyValue(const DomNode* value)
		{
			if (IsScalarNode(value) || IsEmptyArrayNode(value) || IsEmptyObjectNode(value))
			{
				Context.Text += ": ";
				WriteValue(value);
				Context.Text += "\n";
				return;
			}

			if (std::holds_alternative<DomArray>(value->NodeData))
			{
				const auto& arr = std::get<DomArray>(value->NodeData);
				ValidateNoMixedArrays(arr);

				if (CanInlineScalarArray(arr))
				{
					Context.Text += ": ";
					WriteArrayInline(arr);
					Context.Text += "\n";
				}
				else
				{
					Context.Text += ":\n";
					Context.Indentation += IndentWidth;
					WriteArrayBlock(arr);
					Context.Indentation -= IndentWidth;
				}
				return;
			}

			const auto& childObj = std::get<DomObject>(value->NodeData);
			if (CanInlineDocumentObjectValue(childObj))
			{
				Context.Text += ": ";
				WriteObjectInline(childObj);
				Context.Text += "\n";
			}
			else
			{
				Context.Text += ":\n";
				Context.Indentation += IndentWidth;
				WriteDocumentObject(childObj);
				Context.Indentation -= IndentWidth;
			}
		}

		void WriteObjectValue(const DomObject& obj)
		{
			if (obj.empty())
			{
				Context.Text += "{}";
				return;
			}

			if (CanInlineScalarObject(obj))
			{
				WriteObjectInline(obj);
				return;
			}

			WriteDocumentObject(obj);
		}

		void WriteObjectInline(const DomObject& obj)
		{
			Context.Text += "{ ";
			for (size_t i = 0; i < obj.size(); i++)
			{
				const auto& [key, valuePtr] = obj.at(i);
				Context.Text += key;
				Context.Text += ": ";
				WriteValue(valuePtr.get());
				if (i + 1 < obj.size())
				{
					Context.Text += ", ";
				}
			}
			Context.Text += " }";
		}

		void WriteArrayValue(const DomArray& arr)
		{
			ValidateNoMixedArrays(arr);

			if (arr.empty())
			{
				Context.Text += "[]";
				return;
			}

			if (CanInlineScalarArray(arr))
			{
				WriteArrayInline(arr);
				return;
			}

			WriteArrayBlock(arr);
		}

		void WriteArrayInline(const DomArray& arr)
		{
			Context.Text += "[";
			for (size_t i = 0; i < arr.size(); i++)
			{
				WriteValue(arr.at(i).get());
				if (i + 1 < arr.size())
				{
					Context.Text += ", ";
				}
			}
			Context.Text += "]";
		}

		void WriteArrayBlock(const DomArray& arr)
		{
			for (size_t i = 0; i < arr.size(); i++)
			{
				const DomNode* el = arr.at(i).get();

				AppendIndent(Context.Text, Context.Indentation);
				Context.Text += "- ";

				if (std::holds_alternative<DomObject>(el->NodeData))
				{
					WriteListItemObject(std::get<DomObject>(el->NodeData));
				}
				else
				{
					WriteValue(el);
					Context.Text += "\n";
				}
			}
		}

		void WriteListItemObject(const DomObject& obj)
		{
			if (obj.empty())
			{
				Context.Text += "{}\n";
				return;
			}

			{
				const auto& [key, value] = obj.at(0);
				Context.Text += key;
				const DomNode* valueNode = value.get();

				if (IsScalarNode(valueNode) || IsEmptyArrayNode(valueNode) || IsEmptyObjectNode(valueNode))
				{
					Context.Text += ": ";
					WriteValue(valueNode);
					Context.Text += "\n";
				}
				else if (std::holds_alternative<DomArray>(valueNode->NodeData) && CanInlineScalarArray(std::get<DomArray>(valueNode->NodeData)))
				{
					Context.Text += ": ";
					WriteArrayInline(std::get<DomArray>(valueNode->NodeData));
					Context.Text += "\n";
				}
				else if (std::holds_alternative<DomObject>(valueNode->NodeData) && CanInlineScalarObject(std::get<DomObject>(valueNode->NodeData)))
				{
					Context.Text += ": ";
					WriteObjectInline(std::get<DomObject>(valueNode->NodeData));
					Context.Text += "\n";
				}
				else
				{
					Context.Text += ":\n";
					Context.Indentation += IndentWidth * 2;
					WriteValue(valueNode);
					Context.Indentation -= IndentWidth * 2;
				}
			}

			const size_t continuationIndent = Context.Indentation + IndentWidth;

			for (size_t i = 1; i < obj.size(); i++)
			{
				const auto& [key, valuePtr] = obj.at(i);
				const DomNode* value = valuePtr.get();

				AppendIndent(Context.Text, continuationIndent);
				Context.Text += key;

				if (IsScalarNode(value) || IsEmptyArrayNode(value) || IsEmptyObjectNode(value))
				{
					Context.Text += ": ";
					WriteValue(value);
					Context.Text += "\n";
				}
				else if (std::holds_alternative<DomObject>(value->NodeData) && CanInlineScalarObject(std::get<DomObject>(value->NodeData)))
				{
					Context.Text += ": ";
					WriteObjectInline(std::get<DomObject>(value->NodeData));
					Context.Text += "\n";
				}
				else if (std::holds_alternative<DomArray>(value->NodeData) && CanInlineScalarArray(std::get<DomArray>(value->NodeData)))
				{
					Context.Text += ": ";
					WriteArrayInline(std::get<DomArray>(value->NodeData));
					Context.Text += "\n";
				}
				else
				{
					Context.Text += ":\n";
					Context.Indentation = continuationIndent + IndentWidth;
					WriteValue(value);
					Context.Indentation = continuationIndent - IndentWidth;
				}
			}
		}
	};

	enum class LineKind
	{
		Empty,
		KeyValue,
		KeyOnly,
		ListItem
	};

	struct LineToken
	{
		LineKind Kind = LineKind::Empty;
		size_t Indent = 0;
		std::string_view Key;
		std::string_view Value;
	};

	struct Lexer
	{
		explicit Lexer(std::string_view input)
			: Input(input)
		{ }

		bool HasMore() const { return HasPeek || Offset < Input.size(); }

		LineToken Peek()
		{
			if (!HasPeek)
			{
				PeekToken = NextToken();
				HasPeek = true;
			}
			return PeekToken;
		}

		LineToken Read()
		{
			if (HasPeek)
			{
				HasPeek = false;
				return PeekToken;
			}
			return NextToken();
		}

	private:
		LineToken NextToken()
		{
			const size_t lineStart = Offset;
			const size_t lineEnd = Input.find('\n', Offset);
			const size_t lineLimit = (lineEnd == std::string_view::npos) ? Input.size() : lineEnd;

			Offset = (lineEnd == std::string_view::npos) ? Input.size() : (lineEnd + 1);

			size_t indent = 0;
			while (lineStart + indent < lineLimit && Input[lineStart + indent] == ' ')
			{
				indent++;
			}

			if (indent % IndentWidth != 0)
			{
				throw std::runtime_error("LmlDomCodec: Indentation must be a multiple of 2 spaces.");
			}

			if (lineStart + indent >= lineLimit)
			{
				return LineToken { LineKind::Empty, indent };
			}

			auto trimView = [](std::string_view raw)
			{
				while (!raw.empty() && raw.front() == ' ')
				{
					raw.remove_prefix(1);
				}
				while (!raw.empty() && raw.back() == ' ')
				{
					raw.remove_suffix(1);
				}
				return raw;
			};

			std::string_view line = Input.substr(lineStart + indent, lineLimit - (lineStart + indent));
			if (!line.empty() && line.front() == '-')
			{
				std::string_view value = {};
				if (line.size() >= 2 && line[1] == ' ')
				{
					value = line.substr(2);
				}
				else if (line.size() > 1)
				{
					value = line.substr(1);
				}
				value = trimView(value);
				return LineToken { LineKind::ListItem, indent, {}, value };
			}

			bool inQuotes = false;
			bool escape = false;
			size_t colonPos = std::string_view::npos;
			for (size_t i = 0; i < line.size(); i++)
			{
				const char c = line[i];
				if (escape)
				{
					escape = false;
					continue;
				}
				if (c == '\\')
				{
					escape = true;
					continue;
				}
				if (c == '"')
				{
					inQuotes = !inQuotes;
					continue;
				}
				if (!inQuotes && c == ':')
				{
					colonPos = i;
					break;
				}
			}

			if (colonPos == std::string_view::npos)
			{
				std::string_view keyOnly = trimView(line);
				return LineToken { LineKind::KeyOnly, indent, keyOnly, {} };
			}

			std::string_view key = trimView(line.substr(0, colonPos));
			std::string_view value = line.substr(colonPos + 1);
			if (!value.empty() && value.front() == ' ')
			{
				value.remove_prefix(1);
			}
			value = trimView(value);

			if (value.empty())
			{
				return LineToken { LineKind::KeyOnly, indent, key, {} };
			}
			return LineToken { LineKind::KeyValue, indent, key, value };
		}

		std::string_view Input;
		size_t Offset = 0;
		bool HasPeek = false;
		LineToken PeekToken {};
	};

	struct Parser
	{
		explicit Parser(std::string_view input)
			: LexerInstance(input)
		{ }

		DomNode ReadRoot()
		{
			LineToken first = PeekNonEmpty();
			if (first.Kind == LineKind::ListItem)
			{
				throw std::runtime_error("LmlDomCodec: Root must be an object.");
			}
			if (first.Kind == LineKind::Empty && !LexerInstance.HasMore())
			{
				return DomNode { DomObject {} };
			}
			if (first.Indent != 0)
			{
				throw std::runtime_error("LmlDomCodec: Root indentation must be zero.");
			}
			return ReadDocumentObject(0);
		}

	private:
		static std::string_view TrimView(std::string_view raw)
		{
			while (!raw.empty() && raw.front() == ' ')
			{
				raw.remove_prefix(1);
			}
			while (!raw.empty() && raw.back() == ' ')
			{
				raw.remove_suffix(1);
			}
			return raw;
		}

		static bool IsNumberLiteral(std::string_view raw)
		{
			if (raw.empty())
			{
				return false;
			}

			size_t i = 0;
			if (raw[0] == '-')
			{
				if (raw.size() == 1)
				{
					return false;
				}
				i = 1;
			}

			bool sawDigit = false;
			bool sawDot = false;

			for (; i < raw.size(); i++)
			{
				const char c = raw[i];
				if (c >= '0' && c <= '9')
				{
					sawDigit = true;
					continue;
				}
				if (c == '.' && !sawDot)
				{
					sawDot = true;
					continue;
				}
				return false;
			}

			return sawDigit;
		}

		static size_t FindTopLevelChar(std::string_view raw, char needle)
		{
			bool inQuotes = false;
			bool escape = false;
			int depthCurly = 0;
			int depthSquare = 0;

			for (size_t i = 0; i < raw.size(); i++)
			{
				const char c = raw[i];
				if (escape)
				{
					escape = false;
					continue;
				}
				if (c == '\\')
				{
					escape = true;
					continue;
				}
				if (c == '"')
				{
					inQuotes = !inQuotes;
					continue;
				}
				if (!inQuotes)
				{
					if (c == '{')
					{
						depthCurly++;
					}
					else if (c == '}')
					{
						depthCurly--;
					}
					else if (c == '[')
					{
						depthSquare++;
					}
					else if (c == ']')
					{
						depthSquare--;
					}
					else if (c == needle && depthCurly == 0 && depthSquare == 0)
					{
						return i;
					}
				}
			}
			return std::string_view::npos;
		}

		LineToken PeekNonEmpty()
		{
			while (LexerInstance.HasMore())
			{
				const auto next = LexerInstance.Peek();
				if (next.Kind != LineKind::Empty)
				{
					return next;
				}
				LexerInstance.Read();
			}
			return LineToken {};
		}

		DomNode ReadDocumentObject(size_t expectedIndent)
		{
			DomObject obj;

			while (LexerInstance.HasMore())
			{
				const auto next = PeekNonEmpty();
				if (next.Kind == LineKind::Empty || next.Indent < expectedIndent || next.Indent > expectedIndent || next.Kind == LineKind::ListItem)
				{
					break;
				}

				const auto current = LexerInstance.Read();
				if (current.Kind == LineKind::KeyValue)
				{
					DomNode valueNode = ReadValue(current.Value);
					obj.emplace_back(std::string(current.Key), std::make_unique<DomNode>(std::move(valueNode)));
					continue;
				}

				if (current.Kind == LineKind::KeyOnly)
				{
					const auto lookahead = PeekNonEmpty();
					if (lookahead.Kind == LineKind::Empty || lookahead.Indent <= expectedIndent)
					{
						throw std::runtime_error("LmlDomCodec: Key without block value.");
					}

					if (lookahead.Kind == LineKind::ListItem && lookahead.Indent == expectedIndent + IndentWidth)
					{
						DomNode arrayNode = ReadBlockArray(expectedIndent + IndentWidth);
						obj.emplace_back(std::string(current.Key), std::make_unique<DomNode>(std::move(arrayNode)));
						continue;
					}

					DomNode childObj = ReadDocumentObject(expectedIndent + IndentWidth);
					obj.emplace_back(std::string(current.Key), std::make_unique<DomNode>(std::move(childObj)));
				}
			}

			return DomNode { std::move(obj) };
		}

		DomNode ReadValue(std::string_view raw)
		{
			raw = TrimView(raw);
			if (raw == "{}")
			{
				return DomNode { DomObject {} };
			}
			if (raw == "[]")
			{
				return DomNode { DomArray {} };
			}
			if (!raw.empty() && raw.front() == '{' && raw.back() == '}')
			{
				return ReadInlineObject(raw);
			}
			if (!raw.empty() && raw.front() == '[' && raw.back() == ']')
			{
				return ReadInlineArray(raw);
			}
			return DomNode { ReadScalar(raw) };
		}

		DomNode ReadInlineObject(std::string_view raw)
		{
			if (raw.size() >= 2 && raw.front() == '{' && raw.back() == '}')
			{
				raw.remove_prefix(1);
				raw.remove_suffix(1);
			}
			raw = TrimView(raw);
			DomObject obj;
			if (raw.empty())
			{
				return DomNode { std::move(obj) };
			}

			while (!raw.empty())
			{
				const size_t comma = FindTopLevelChar(raw, ',');
				std::string_view pair = (comma == std::string_view::npos) ? raw : raw.substr(0, comma);
				pair = TrimView(pair);
				if (pair.empty())
				{
					throw std::runtime_error("LmlDomCodec: Inline object contains empty pair.");
				}

				const size_t colon = FindTopLevelChar(pair, ':');
				if (colon == std::string_view::npos)
				{
					throw std::runtime_error("LmlDomCodec: Inline object pair missing ':'");
				}

				std::string_view key = TrimView(pair.substr(0, colon));
				std::string_view value = pair.substr(colon + 1);
				value = TrimView(value);

				DomNode valueNode = ReadValue(value);
				obj.emplace_back(std::string(key), std::make_unique<DomNode>(std::move(valueNode)));

				if (comma == std::string_view::npos)
				{
					break;
				}
				raw.remove_prefix(comma + 1);
				raw = TrimView(raw);
				if (raw.empty())
				{
					throw std::runtime_error("LmlDomCodec: Inline object has trailing comma.");
				}
			}

			return DomNode { std::move(obj) };
		}

		DomNode ReadInlineArray(std::string_view raw)
		{
			if (raw.size() >= 2 && raw.front() == '[' && raw.back() == ']')
			{
				raw.remove_prefix(1);
				raw.remove_suffix(1);
			}
			raw = TrimView(raw);
			DomArray arr;
			if (raw.empty())
			{
				return DomNode { std::move(arr) };
			}

			while (!raw.empty())
			{
				const size_t comma = FindTopLevelChar(raw, ',');
				std::string_view element = (comma == std::string_view::npos) ? raw : raw.substr(0, comma);
				element = TrimView(element);
				if (element.empty())
				{
					throw std::runtime_error("LmlDomCodec: Inline array contains empty element.");
				}

				DomNode elementNode = ReadValue(element);
				arr.push_back(std::make_unique<DomNode>(std::move(elementNode)));

				if (comma == std::string_view::npos)
				{
					break;
				}
				raw.remove_prefix(comma + 1);
				raw = TrimView(raw);
				if (raw.empty())
				{
					throw std::runtime_error("LmlDomCodec: Inline array has trailing comma.");
				}
			}

			ValidateNoMixedArrays(arr);
			return DomNode { std::move(arr) };
		}

		DomNode ReadBlockArray(size_t expectedIndent)
		{
			DomArray arr;

			while (LexerInstance.HasMore())
			{
				const auto next = PeekNonEmpty();
				if (next.Kind != LineKind::ListItem || next.Indent != expectedIndent)
				{
					break;
				}

				const auto item = LexerInstance.Read();
				const auto raw = TrimView(item.Value);

				if (raw.empty())
				{
					const auto lookahead = PeekNonEmpty();
					if (lookahead.Kind == LineKind::Empty || lookahead.Indent != expectedIndent + IndentWidth)
					{
						throw std::runtime_error("LmlDomCodec: Invalid indentation for block list item.");
					}
					if (lookahead.Kind == LineKind::ListItem)
					{
						DomNode nestedArray = ReadBlockArray(expectedIndent + IndentWidth);
						arr.push_back(std::make_unique<DomNode>(std::move(nestedArray)));
					}
					else
					{
						DomNode nestedObj = ReadDocumentObject(expectedIndent + IndentWidth);
						arr.push_back(std::make_unique<DomNode>(std::move(nestedObj)));
					}
					continue;
				}

				const size_t colon = FindTopLevelChar(raw, ':');
				if (colon != std::string_view::npos)
				{
					DomObject obj;
					std::string_view key = TrimView(raw.substr(0, colon));
					std::string_view valueRaw = TrimView(raw.substr(colon + 1));
					DomNode valueNode = ReadValue(valueRaw);
					obj.emplace_back(std::string(key), std::make_unique<DomNode>(std::move(valueNode)));

					while (LexerInstance.HasMore())
					{
						const auto nextPair = PeekNonEmpty();
						if (nextPair.Kind == LineKind::Empty || nextPair.Indent != expectedIndent + IndentWidth || nextPair.Kind == LineKind::ListItem)
						{
							break;
						}

						const auto pairToken = LexerInstance.Read();
						if (pairToken.Kind == LineKind::KeyValue)
						{
							DomNode pairValue = ReadValue(pairToken.Value);
							obj.emplace_back(std::string(pairToken.Key), std::make_unique<DomNode>(std::move(pairValue)));
							continue;
						}

						if (pairToken.Kind == LineKind::KeyOnly)
						{
							const auto lookahead = PeekNonEmpty();
							if (lookahead.Kind == LineKind::ListItem && lookahead.Indent == expectedIndent + IndentWidth + IndentWidth)
							{
								DomNode childArray = ReadBlockArray(expectedIndent + IndentWidth + IndentWidth);
								obj.emplace_back(std::string(pairToken.Key), std::make_unique<DomNode>(std::move(childArray)));
							}
							else
							{
								DomNode childObj = ReadDocumentObject(expectedIndent + IndentWidth + IndentWidth);
								obj.emplace_back(std::string(pairToken.Key), std::make_unique<DomNode>(std::move(childObj)));
							}
						}
					}

					arr.push_back(std::make_unique<DomNode>(DomNode { std::move(obj) }));
					continue;
				}

				DomNode valueNode = ReadValue(raw);
				arr.push_back(std::make_unique<DomNode>(std::move(valueNode)));
			}

			ValidateNoMixedArrays(arr);
			return DomNode { std::move(arr) };
		}

		DomValue ReadScalar(std::string_view raw)
		{
			raw = TrimView(raw);

			if (raw.empty())
			{
				return DomValue { std::string {} };
			}

			if (raw.front() == '"' && raw.back() != '"')
			{
				throw std::runtime_error("LmlDomCodec: Unterminated quoted string.");
			}

			if (raw.front() == '"' && raw.back() == '"')
			{
				std::string out;
				out.reserve(raw.size());

				bool escape = false;
				for (size_t i = 1; i + 1 < raw.size(); i++)
				{
					char c = raw[i];

					if (escape)
					{
						switch (c)
						{
							case 'n': out.push_back('\n'); break;
							case '"': out.push_back('"'); break;
							case '\\': out.push_back('\\'); break;
							default: out.push_back(c); break;
						}
						escape = false;
						continue;
					}

					if (c == '\\')
					{
						escape = true;
						continue;
					}

					out.push_back(c);
				}

				return DomValue { std::move(out) };
			}

			if (raw == "null")
			{
				return DomValue { std::monostate {} };
			}
			if (raw == "true")
			{
				return DomValue { true };
			}
			if (raw == "false")
			{
				return DomValue { false };
			}

			if (!IsNumberLiteral(raw))
			{
				return DomValue { std::string(raw) };
			}

			const bool isFloat = raw.find('.') != std::string_view::npos;
			if (isFloat)
			{
				const double value = std::stod(std::string(raw));
				return DomValue { value };
			}

			if (raw.front() == '-')
			{
				const long long s = std::stoll(std::string(raw));
				if (s < std::numeric_limits<int64_t>::min() || s > std::numeric_limits<int64_t>::max())
				{
					throw std::runtime_error("LmlDomCodec: Signed integer literal out of range for int64_t.");
				}
				return DomValue { static_cast<int64_t>(s) };
			}

			const unsigned long long u = std::stoull(std::string(raw));
			if (u <= std::numeric_limits<uint64_t>::max())
			{
				return DomValue { static_cast<uint64_t>(u) };
			}

			throw std::runtime_error("LmlDomCodec: Unsigned integer literal out of range for uint64_t.");
		}

		Lexer LexerInstance;
	};
}

namespace Ludus::Engine::Serialization::Codecs
{
	std::string LmlDomCodec::Encode(const Ludus::Engine::Serialization::Core::DomNode& node)
	{
		WriterContext context;
		Writer writer { context };

		writer.WriteRoot(&node);
		return context.Text;
	}

	Ludus::Engine::Serialization::Core::DomNode LmlDomCodec::Decode(const std::string& data)
	{
		Parser parser(data);
		return parser.ReadRoot();
	}
}
