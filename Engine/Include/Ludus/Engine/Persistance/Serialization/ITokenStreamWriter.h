#pragma once

#include <Ludus/Engine/Persistance/Serialization/Token.h>

namespace Ludus::Engine::Persistance::Serialization
{
	struct ITokenStreamWriter
	{
		virtual ~ITokenStreamWriter() = default;

		virtual void Emit(const Token& token) = 0;
	};
}
