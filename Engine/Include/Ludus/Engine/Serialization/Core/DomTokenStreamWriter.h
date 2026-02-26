#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::Engine::Serialization::Core
{
	struct DomTokenStreamWriter final : public ITokenStreamWriter
	{
	private:
		DomDocument& m_Document;
		std::vector<DomNode*> m_NodeStack;
		std::optional<std::string> m_PendingKey;

		DomNode* AttachNode(DomNodePtr node);

	public:
		DomTokenStreamWriter(DomDocument& document);
		virtual void Emit(const Token& token) override;
	};
}
