#pragma once

#include <string>

#include <Ludus/Engine/Persistance/Serialization/DomNode.h>

namespace Ludus::Engine::Persistance::Codecs
{
	class IDomTextCodec
	{
	public:
		virtual ~IDomTextCodec() = default;

		virtual std::string Encode(const Ludus::Engine::Persistance::Serialization::DomNode& node) = 0;
		virtual Ludus::Engine::Persistance::Serialization::DomNode Decode(const std::string& data) = 0;
	};
}
