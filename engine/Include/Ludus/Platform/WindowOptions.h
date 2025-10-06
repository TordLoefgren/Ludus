#pragma once

#include <string>

namespace Ludus::Platform
{
	struct WindowOptions
	{
		int Width;
		int Height;
		std::string Title;
		bool resizeable = true;
	};
}
