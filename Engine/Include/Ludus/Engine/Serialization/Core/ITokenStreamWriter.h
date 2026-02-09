#pragma once

#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Engine::Serialization::Core
{
	struct ITokenStreamWriter
	{
		virtual ~ITokenStreamWriter() = default;

		virtual void Emit(const Token& token) = 0;
	};
}
