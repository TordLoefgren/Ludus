#include "pch.h"

#include <format>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/UI/Containers.h>
#include <Ludus/UI/Widgets.h>

namespace Ludus::Editor::Panels
{
	void ConsolePanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags | ImGuiWindowFlags_HorizontalScrollbar;
		auto windowTitle = CreateWindowTitle("Console");

		if (Ludus::UI::Containers::Window window(windowTitle.c_str(), &m_Open, flags); window)
		{
			auto& logEntries = Ludus::Engine::Debug::GetLogEntries();

			for (auto& entry : logEntries)
			{
				auto formattetText = FormatEntry(entry);

				auto iter = m_TextToIndex.find(formattetText);
				if (iter == m_TextToIndex.end())
				{
					auto size = m_AggregateText.size();;
					m_AggregateText.push_back({ std::move(formattetText), 1 });
					m_TextToIndex[m_AggregateText.back().Text] = size;
				}
				else
				{
					m_AggregateText.at(iter->second).Count++;
				}
			}

			for (auto& text : m_AggregateText)
			{
				if (text.Count == 1)
				{
					Ludus::UI::Widgets::Text(text.Text);
				}
				else
				{
					Ludus::UI::Widgets::Text(std::format("{} ({})", text.Text, text.Count));
				}
			}

			Ludus::UI::Utilities::ScrollToNewest();

			logEntries.clear();
		}
	}

	std::string ConsolePanel::FormatEntry(const Ludus::Engine::Debug::LogEntry& entry)
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
