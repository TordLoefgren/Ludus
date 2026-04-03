#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>
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

		std::vector<AggregateText> m_AggregateText;
		std::unordered_map<std::string, size_t> m_TextToIndex;

		std::string FormatEntry(const Ludus::Engine::Debug::LogEntry& entry);

	public:
		virtual Ludus::Editor::Panels::PanelKind GetPanelKind() const override { return Ludus::Editor::Panels::PanelKind::Console; }
		virtual bool UsesVisibilityState() const override { return true; }

		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;

		void Clear();
	};
}
