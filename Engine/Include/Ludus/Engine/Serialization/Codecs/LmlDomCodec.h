#pragma once

#include <string>

#include <Ludus/Engine/Serialization/Codecs/IDomTextCodec.h>

namespace Ludus::Engine::Serialization::Codecs
{
	class LmlDomCodec final : public IDomTextCodec
	{
	public:
		LmlDomCodec() = default;

		std::string Encode(const Ludus::Engine::Serialization::Core::DomNode& node) override;
		Ludus::Engine::Serialization::Core::DomNode Decode(const std::string& data) override;
	};
}
