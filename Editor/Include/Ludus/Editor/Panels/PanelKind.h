#pragma once

#include <stdexcept>
#include <string_view>

namespace Ludus::Editor::Panels
{
	enum class PanelKind
	{
		Console,
		Content,
		Dock,
		Hierarchy,
		Inspector,
		ImGuiDemo,
		Viewport
	};

	constexpr std::string_view ToString(const PanelKind panelKind)
	{
		switch (panelKind)
		{
			case PanelKind::Console:		return "Console";
			case PanelKind::Content:		return "Content";
			case PanelKind::Dock:			return "Dock";
			case PanelKind::Hierarchy:		return "Hierarchy";
			case PanelKind::Inspector:		return "Inspector";
			case PanelKind::ImGuiDemo:		return "ImGuiDemo";
			case PanelKind::Viewport:		return "Viewport";
			default:						throw std::runtime_error("Unsupported panel kind.");
		}
	}
}
