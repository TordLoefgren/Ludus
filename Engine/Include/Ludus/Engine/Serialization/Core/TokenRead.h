#pragma once

#include <format>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>

namespace Ludus::Engine::Serialization::Core
{
	using Token = Ludus::Engine::Serialization::Core::Token;

	inline std::string_view TokenTypeName(const Token& token)
	{
		return std::visit(Overloaded
			{
				[](const Token::StartObject&) -> std::string_view { return "StartObject"; },
				[](const Token::EndObject&) -> std::string_view { return "EndObject"; },
				[](const Token::StartArray&) -> std::string_view { return "StartArray"; },
				[](const Token::EndArray&) -> std::string_view { return "EndArray"; },
				[](const Token::Key&) -> std::string_view { return "Key"; },
				[](const Token::Null&) -> std::string_view { return "Null"; },
				[](const Token::Bool&) -> std::string_view { return "Bool"; },
				[](const Token::Int&) -> std::string_view { return "Int"; },
				[](const Token::Uint&) -> std::string_view { return "Uint"; },
				[](const Token::Double&) -> std::string_view { return "Double"; },
				[](const Token::String&) -> std::string_view { return "String"; },
			}, token.Data);
	}

	template<typename T>
	inline std::string_view TokenTypeName()
	{
		if constexpr (std::is_same_v<T, Token::StartObject>) return "StartObject";
		if constexpr (std::is_same_v<T, Token::EndObject>) return "EndObject";
		if constexpr (std::is_same_v<T, Token::StartArray>) return "StartArray";
		if constexpr (std::is_same_v<T, Token::EndArray>) return "EndArray";
		if constexpr (std::is_same_v<T, Token::Key>) return "Key";
		if constexpr (std::is_same_v<T, Token::Null>) return "Null";
		if constexpr (std::is_same_v<T, Token::Bool>) return "Bool";
		if constexpr (std::is_same_v<T, Token::Int>) return "Int";
		if constexpr (std::is_same_v<T, Token::Uint>) return "Uint";
		if constexpr (std::is_same_v<T, Token::Double>) return "Double";
		if constexpr (std::is_same_v<T, Token::String>) return "String";
		return "Unknown";
	}

	inline SerializationException WithContext(const SerializationException& ex, std::string_view context)
	{
		return SerializationException(std::format("{}: {}", context, ex.what()));
	}

	template<typename T>
	inline bool Is(const Token& token)
	{
		return std::holds_alternative<T>(token.Data);
	}

	template<typename T>
	inline const T& As(const Token& token)
	{
		if (!Is<T>(token))
		{
			throw SerializationException(
				std::format("Unexpected token type. Expected '{}', got '{}'.", TokenTypeName<T>(), TokenTypeName(token))
			);
		}

		return std::get<T>(token.Data);
	}

	template<typename T>
	inline T ConsumeAs(ITokenStreamReader& reader)
	{
		const Token& token = reader.Peek();
		const T value = As<T>(token);

		reader.Consume();

		return value;
	}

	inline int ConsumeIntLike(ITokenStreamReader& reader)
	{
		if (Is<Token::Int>(reader.Peek()))
		{
			const auto value = ConsumeAs<Token::Int>(reader).Data;
			if (value < static_cast<int64_t>(std::numeric_limits<int>::min()) ||
				value > static_cast<int64_t>(std::numeric_limits<int>::max()))
			{
				throw SerializationException("Integer literal out of range for int.");
			}
			return static_cast<int>(value);
		}
		if (Is<Token::Uint>(reader.Peek()))
		{
			const auto value = ConsumeAs<Token::Uint>(reader).Data;
			if (value > static_cast<uint64_t>(std::numeric_limits<int>::max()))
			{
				throw SerializationException("Integer literal out of range for int.");
			}
			return static_cast<int>(value);
		}

		throw SerializationException("Expected integer token.");
	}

	inline uint32_t ConsumeUint32Like(ITokenStreamReader& reader)
	{
		if (Is<Token::Uint>(reader.Peek()))
		{
			const auto value = ConsumeAs<Token::Uint>(reader).Data;
			if (value > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
			{
				throw SerializationException("Unsigned integer literal out of range for uint32_t.");
			}
			return static_cast<uint32_t>(value);
		}
		if (Is<Token::Int>(reader.Peek()))
		{
			const auto value = ConsumeAs<Token::Int>(reader).Data;
			if (value < 0 || value > static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))
			{
				throw SerializationException("Unsigned integer literal out of range for uint32_t.");
			}
			return static_cast<uint32_t>(value);
		}

		throw SerializationException("Expected unsigned integer token.");
	}

	inline uint64_t ConsumeUint64Like(ITokenStreamReader& reader)
	{
		if (Is<Token::Uint>(reader.Peek()))
		{
			return ConsumeAs<Token::Uint>(reader).Data;
		}
		if (Is<Token::Int>(reader.Peek()))
		{
			const auto value = ConsumeAs<Token::Int>(reader).Data;
			if (value < 0)
			{
				throw SerializationException("Expected non-negative integer token.");
			}
			return static_cast<uint64_t>(value);
		}

		throw SerializationException("Expected unsigned integer token.");
	}

	inline float ConsumeFloatLike(ITokenStreamReader& reader)
	{
		if (Is<Token::Double>(reader.Peek()))
		{
			return static_cast<float>(ConsumeAs<Token::Double>(reader).Data);
		}
		if (Is<Token::Int>(reader.Peek()))
		{
			return static_cast<float>(ConsumeAs<Token::Int>(reader).Data);
		}
		if (Is<Token::Uint>(reader.Peek()))
		{
			return static_cast<float>(ConsumeAs<Token::Uint>(reader).Data);
		}

		throw SerializationException("Expected floating-point token.");
	}

	template<typename T>
	inline bool TryConsume(ITokenStreamReader& reader, T* out = nullptr)
	{
		if (!Is<T>(reader.Peek()))
		{
			return false;
		}

		if (out)
		{
			*out = std::get<T>(reader.Peek().Data);
		}

		reader.Consume();

		return true;
	}

	inline std::string_view ReadKey(ITokenStreamReader& reader)
	{
		return ConsumeAs<Token::Key>(reader).Data;
	}

	inline void SkipValue(ITokenStreamReader& reader)
	{
		const Token& token = reader.Peek();

		if (Is<Token::StartObject>(token))
		{
			// StartObject.
			reader.Consume();

			// Key-Value pairs.
			while (!Is<Token::EndObject>(reader.Peek()))
			{
				ConsumeAs<Token::Key>(reader);
				SkipValue(reader);
			}

			// EndObject.
			reader.Consume();

			return;
		}

		if (Is<Token::StartArray>(token))
		{
			// StartObject.
			reader.Consume();

			// Key-Value pairs.
			while (!Is<Token::EndArray>(reader.Peek()))
			{
				SkipValue(reader);
			}

			// EndObject.
			reader.Consume();

			return;
		}

		reader.Consume();
	}

	template<typename TMemberFunction>
	inline void ReadObject(ITokenStreamReader& reader, TMemberFunction&& onMember)
	{
		ConsumeAs<Token::StartObject>(reader);

		while (!Is<Token::EndObject>(reader.Peek()))
		{
			const std::string_view key = ReadKey(reader);
			try
			{
				onMember(key);
			}
			catch (const SerializationException& ex)
			{
				throw WithContext(ex, std::format("at key '{}'", key));
			}
		}

		ConsumeAs<Token::EndObject>(reader);
	}
}
