#pragma once

#include <string>

struct WindowOptions
{
	int Width;
	int Height;
	std::string Title;
	bool resizeable = true;
};
