#pragma once

#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/WindowOptions.h>

namespace Ludus::Platform
{
	struct WindowUserData
	{
		Ludus::Platform::Input* Input;
		WindowOptions Options;
	};
}