#include "pch.h"

#include <optional>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/UI/Context/DockingContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/MenuBarScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
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

	void DockPanel::DrawToolBar(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto buttonWidth = 30.0f;
		const auto spacing = 6.0f;
		const auto buttonCount = 3;

		const auto totalWidth = buttonCount * buttonWidth + (buttonCount - 1) * spacing;

		const auto availableWidth = Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X;
		Ludus::UI::Context::WindowContext::SetCursorPositionX((availableWidth - totalWidth) * 0.5f);

		auto& executionManager = context.EditorContext.State.ExecutionManager;
		const auto activeColor = Ludus::UI::Scope::GetStyleColor(Ludus::UI::Scope::Color::ButtonHovered);

		auto startLabel = Ludus::UI::CreateLabel(ICON_PLAY, "Start");

		if (m_IsPlayButtonPressed)
		{
			Ludus::UI::Scope::StyleColorScope colorScope(
				{
					Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::Button, activeColor),
					Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::ButtonHovered, activeColor),
					Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::ButtonActive, activeColor)
				}
			);

			if (Ludus::UI::Widgets::Button(startLabel.c_str(), { buttonWidth, buttonWidth }))
			{
				executionManager.Apply(context.SystemContext, Ludus::Editor::Core::ExecutionMode::Start);
				m_IsPlayButtonPressed = true;
				m_IsPauseButtonPressed = false;
			}
		}
		else
		{
			if (Ludus::UI::Widgets::Button(startLabel.c_str(), { buttonWidth, buttonWidth }))
			{
				executionManager.Apply(context.SystemContext, Ludus::Editor::Core::ExecutionMode::Start);
				m_IsPlayButtonPressed = true;
				m_IsPauseButtonPressed = false;
			}
		}

		Ludus::UI::Context::LayoutContext::SameLine(0.0f, spacing);

		auto stopLabel = Ludus::UI::CreateLabel(ICON_STOP, "Stop");

		if (Ludus::UI::Widgets::Button(stopLabel.c_str(), { buttonWidth, buttonWidth }))
		{
			executionManager.Apply(context.SystemContext, Ludus::Editor::Core::ExecutionMode::Stop);

			m_IsPlayButtonPressed = false;
			m_IsPauseButtonPressed = false;
		}

		Ludus::UI::Context::LayoutContext::SameLine(0.0f, spacing);

		auto pauseLabel = Ludus::UI::CreateLabel(ICON_PAUSE, "Pause");

		if (m_IsPauseButtonPressed)
		{
			Ludus::UI::Scope::StyleColorScope colorScope(
				{
					Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::Button, activeColor),
					Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::ButtonHovered, activeColor),
					Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::ButtonActive, activeColor)
				}
			);

			if (Ludus::UI::Widgets::Button(pauseLabel.c_str(), { buttonWidth, buttonWidth }))
			{
				executionManager.Apply(context.SystemContext, Ludus::Editor::Core::ExecutionMode::Pause);

				m_IsPauseButtonPressed = true;
				m_IsPlayButtonPressed = false;
			}
		}
		else
		{
			if (Ludus::UI::Widgets::Button(pauseLabel.c_str(), { buttonWidth, buttonWidth }) && m_IsPlayButtonPressed)
			{
				executionManager.Apply(context.SystemContext, Ludus::Editor::Core::ExecutionMode::Pause);

				m_IsPauseButtonPressed = true;
				m_IsPlayButtonPressed = false;
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
			DrawToolBar(context);

			Ludus::UI::Context::DockingContext::CreateDockSpace(windowTitle.c_str(), { 0.0f, 0.0f }, Ludus::UI::Flags::DockNode::None | Ludus::UI::Flags::DockNodeInternal::NoWindowMenuButton);
		}

		return true;
	}
}
