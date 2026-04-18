#pragma once

#include <string_view>

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Editor::Widgets
{
	struct WidgetState
	{
		bool Active;
		bool Hovered;
		bool Pressed;
		Ludus::Engine::Math::Vector2D Min;
		Ludus::Engine::Math::Vector2D Max;
	};

	struct RecentProjectRowData
	{
		std::string_view Id;
		std::string_view DisplayName;
		std::string_view Path;
		bool IsPathMissing = false;
	};

	WidgetState ButtonBase(const char* id, Ludus::Engine::Math::Vector2D size);

	bool RecentProjectRow(const RecentProjectRowData& rowData);
}
