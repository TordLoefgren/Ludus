#include "pch.h"

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/UI/Context/DockingContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>

namespace Ludus::Editor::Panels
{
	void DockPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
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
			Ludus::UI::Context::DockingContext::CreateDockSpace(windowTitle.c_str());
		}
	}
}
