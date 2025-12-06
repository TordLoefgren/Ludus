#include "Ludus/Editor/Panels/ConsolePanel.h"

namespace Ludus::Editor::Panels
{
	void ConsolePanel::DrawPanel()
	{
		static bool open = true;
		if (Ludus::UI::Containers::Window window("Console", &open, Constants::PanelFlags); window)
		{
			auto& logEntries = Ludus::Engine::Debug::GetLogEntries();
			for (auto& entry : logEntries)
			{
				Ludus::UI::Widgets::Text(FormatEntry(entry));
			}
		}
	}

	std::string ConsolePanel::FormatEntry(Ludus::Engine::Debug::LogEntry& entry)
	{
		if (entry.Tag.empty())
		{
			return std::format(
				"[{}] {}({}): {}",
				ToString(entry.Level),
				entry.File,
				entry.Line,
				entry.Message
			);
		}

		return std::format(
			"{}({}): [{}][{}] {}",
			Ludus::Engine::Debug::ToString(entry.Level),
			entry.File,
			entry.Line,
			entry.Tag,
			entry.Message
		);
	}
}
