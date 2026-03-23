#include "pch.h"

#include <format>
#include <utility>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ConsolePanel.h>
#include <Ludus/UI/Context/ScrollContext.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Panels
{
	bool ConsolePanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags | Ludus::UI::Flags::Window::HorizontalScrollbar;
		auto windowTitle = CreateWindowTitle("Console");

		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			auto& logEntries = Ludus::Engine::Debug::GetLogEntries();

			for (auto& entry : logEntries)
			{
				auto formattetText = FormatEntry(entry);

				auto iter = m_TextToIndex.find(formattetText);
				if (iter == m_TextToIndex.end())
				{
					auto size = m_AggregateText.size();
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
					Ludus::UI::Widgets::TextUnformatted(text.Text);
				}
				else
				{
					Ludus::UI::Widgets::TextUnformatted(std::format("{} ({})", text.Text, text.Count));
				}
			}

			Ludus::UI::Context::ScrollContext::ScrollToNewest();

			logEntries.clear();
		}

		return true;
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

	void ConsolePanel::Clear()
	{
		m_AggregateText.clear();
		m_TextToIndex.clear();
	}
}
