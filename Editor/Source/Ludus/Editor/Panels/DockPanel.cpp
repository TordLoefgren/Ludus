#include "pch.h"

#include <optional>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/UI/Context/DockingContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/MenuBarScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Text.h>
#include <Ludus/UI/Widgets/Toggle.h>

namespace Ludus::Editor::Panels
{
	void DockPanel::DrawMenuBar(Ludus::Editor::Panels::PanelContext& context)
	{
		if (Ludus::UI::Scope::MenuBarScope menuBar; menuBar)
		{
			if (Ludus::UI::Scope::MenuScope fileMenu("File"); fileMenu)
			{
				if (Ludus::UI::Widgets::MenuItem("New"))
				{
					// Not implemented.
				}

				if (Ludus::UI::Widgets::MenuItem("Open"))
				{
					// Not implemented.
				}
			}

			if (Ludus::UI::Scope::MenuScope viewMenu("View"); viewMenu)
			{
				if (Ludus::UI::Widgets::MenuItem("Add viewport"))
				{
					context.EditorContext.State.Requests.AddViewport = true;
				}

				if (Ludus::UI::Scope::MenuScope panelsMenu("Panels"); panelsMenu)
				{
					Ludus::UI::Widgets::Checkbox("Console", &context.ActivePanelState.ShowConsolePanel);
					Ludus::UI::Widgets::Checkbox("Hierarchy", &context.ActivePanelState.ShowHierachyPanel);
					Ludus::UI::Widgets::Checkbox("ImGuiDemo", &context.ActivePanelState.ShowImGuiDemoPanel);
					Ludus::UI::Widgets::Checkbox("Inspector", &context.ActivePanelState.ShowInspectorPanel);
				}
			}
		}
	}

	bool DockPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto viewport = Ludus::UI::Context::ViewportContext::GetMainViewport();
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(viewport.WorkPosition);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(viewport.WorkSize);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		Ludus::UI::Scope::StyleVarScope styleVar(
			{
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 0.0f),
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 0.0f)
			}
		);

		// The dock panel should never close, as it enables docking for all other panels.
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags;
		auto windowTitle = CreateWindowTitle("DockPanel");

		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), nullptr, Ludus::Editor::Core::Constants::DockPanelWindowFlags); window)
		{
			DrawMenuBar(context);

			Ludus::UI::Context::DockingContext::CreateDockSpace(windowTitle.c_str(), { 0.0f, 0.0f }, Ludus::UI::Flags::DockNode::None | Ludus::UI::Flags::DockNodeInternal::NoWindowMenuButton);
		}

		return true;
	}
}
