#pragma once

#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>

namespace Ludus::Editor::Dialogs
{
	inline void CenterNextDialogOnMainViewport()
	{
		const auto viewport = Ludus::UI::Context::ViewportContext::GetMainViewport();
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(
			viewport.GetCenter(),
			Ludus::UI::Context::WindowContext::WindowCondition::Appearing,
			{ 0.5f, 0.5f }
		);
	}

	inline void CenterNextDialogOnMousePosition()
	{
		const auto mousePosition = Ludus::UI::Context::InputContext::MousePosition();
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(
			mousePosition,
			Ludus::UI::Context::WindowContext::WindowCondition::Appearing,
			{ 0.5f, 0.5f }
		);
	}
}
