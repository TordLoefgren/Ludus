#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/UI/Containers.h>
#include <Ludus/UI/Widgets.h>

namespace Ludus::Editor::Panels
{
	class ConsolePanel final : public Ludus::Editor::Core::PanelSystem
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

		virtual void DrawPanel() override;
	};
}
