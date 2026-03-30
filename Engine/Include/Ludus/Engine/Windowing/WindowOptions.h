#pragma once

#include <string>
#include <utility>

namespace Ludus::Engine::Windowing
{
	struct WindowOptions
	{
		int StartupWidth = 1920;
		int StartupHeight = 1080;
		std::string Title = "Ludus";
		std::string IconPath = "";
		bool IsResizeable = true;
		bool IsFocused = true;
		bool IsIconified = false;
		bool IsMaximized = false;

		WindowOptions() = default;

		WindowOptions(
			int startupWidth,
			int startupHeight,
			std::string title,
			bool isResizeable = true,
			std::string iconPath = ""
		) :
			StartupWidth(startupWidth),
			StartupHeight(startupHeight),
			Title(std::move(title)),
			IconPath(std::move(iconPath)),
			IsResizeable(isResizeable)
		{ }
	};
}
