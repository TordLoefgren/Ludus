#pragma once

#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Engine::Serialization::Core
{
	struct ITokenStreamReader
	{
		virtual ~ITokenStreamReader() = default;

		virtual bool IsComplete() const = 0;
		virtual const Token& Peek() const = 0;
		virtual void Consume() = 0;
	};
}
