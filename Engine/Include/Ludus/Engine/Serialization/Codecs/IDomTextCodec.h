#pragma once

#include <string>

#include <Ludus/Engine/Serialization/Core/DomNode.h>

namespace Ludus::Engine::Serialization::Codecs
{
	class IDomTextCodec
	{
	public:
		virtual ~IDomTextCodec() = default;

		virtual std::string Encode(const Ludus::Engine::Serialization::Core::DomNode& node) = 0;
		virtual Ludus::Engine::Serialization::Core::DomNode Decode(const std::string& data) = 0;
	};
}
