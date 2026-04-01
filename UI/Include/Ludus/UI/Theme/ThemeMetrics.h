#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Theme
{
	struct ThemeMetrics
	{
		Ludus::Engine::Math::Vector2D WindowPadding { 8.0f, 8.0f };
		Ludus::Engine::Math::Vector2D FramePadding { 6.0f, 4.0f };
		Ludus::Engine::Math::Vector2D CellPadding { 6.0f, 4.0f };
		Ludus::Engine::Math::Vector2D ItemSpacing { 8.0f, 6.0f };
		Ludus::Engine::Math::Vector2D ItemInnerSpacing { 6.0f, 4.0f };

		float WindowBorderSize = 1.0f;
		float ChildBorderSize = 1.0f;
		float PopupBorderSize = 1.0f;
		float FrameBorderSize = 1.0f;
		float TabBorderSize = 0.0f;

		float WindowRounding = 4.0f;
		float ChildRounding = 4.0f;
		float FrameRounding = 4.0f;
		float PopupRounding = 4.0f;
		float ScrollbarRounding = 6.0f;
		float GrabRounding = 4.0f;
		float TabRounding = 4.0f;
	};
}
