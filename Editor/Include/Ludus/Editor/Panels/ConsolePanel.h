#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Panels
{
	class ConsolePanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		struct AggregateText
		{
			std::string Text;
			size_t Count;
		};

	private:
		std::string FormatEntry(const Ludus::Engine::Debug::LogEntry& entry);

	public:
		std::vector<AggregateText> m_AggregateText;
		std::unordered_map<std::string, size_t> m_TextToIndex;

		virtual void UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
