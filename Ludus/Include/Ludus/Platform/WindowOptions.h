#pragma once

#include <string>

namespace Ludus::Platform
{
	struct WindowOptions
	{
		int Width = 1024;
		int Height = 768;
		std::string Title = "Ludus";
		bool IsFocused = true;
		bool IsIconified = false;
		bool IsMaximized = false;
		bool IsResizeable = true;
	};
}
