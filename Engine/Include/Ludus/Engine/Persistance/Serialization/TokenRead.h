#pragma once
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>

namespace Ludus::Engine::Persistance::Serialization
{
	using Token = Ludus::Engine::Persistance::Serialization::Token;

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
			throw PersistanceException("Unexpected token type.");
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

	template<class TMemberFunction>
	inline void ReadObject(ITokenStreamReader& reader, TMemberFunction&& onMember)
	{
		ConsumeAs<Token::StartObject>(reader);

		while (!Is<Token::EndObject>(reader.Peek()))
		{
			const std::string_view key = ReadKey(reader);
			onMember(key);
		}

		ConsumeAs<Token::EndObject>(reader);
	}
}
