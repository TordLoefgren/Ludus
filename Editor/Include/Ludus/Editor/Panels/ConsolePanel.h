#pragma once

#include <format>
#include <string>

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
		std::string FormatEntry(Ludus::Engine::Debug::LogEntry& entry);

	public:
		virtual void DrawPanel() override;
	};
}
