#pragma once

#include <Ludus/Engine/Persistance/Serialization/Token.h>

namespace Ludus::Engine::Persistance::Serialization
{
	struct ITokenStreamReader
	{
		virtual ~ITokenStreamReader() = default;

		virtual bool IsComplete() const = 0;
		virtual const Token& Peek() const = 0;
		virtual void Consume() = 0;
	};
}
