#pragma once

#include <string>

namespace Ludus::Engine::Platform
{
	struct WindowOptions
	{
		int Width = 1024;
		int Height = 768;
		std::string Title = "Ludus";
		bool IsResizeable = true;
		bool IsFocused = true;
		bool IsIconified = false;
		bool IsMaximized = false;
	};
}
