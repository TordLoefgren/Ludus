#pragma once

#include <format>
#include <string>

#include <Ludus/Debug/Debug.h>
#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/UI/Containers.h>
#include <Ludus/UI/Widgets.h>

namespace Ludus::Editor::Panels
{
	class ConsolePanel final : public Ludus::Editor::Core::PanelSystem
	{
	private:
		std::string FormatEntry(Ludus::Debug::LogEntry& entry);

	public:
		virtual void DrawPanel() override;
	};
}
