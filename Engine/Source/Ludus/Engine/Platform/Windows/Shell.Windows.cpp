#include "pch.h"

#include <filesystem>
#include <string_view>

#include "WinShellExecute.h"
#include "WinText.h"

#include <Ludus/Engine/Platform/Shell.h>

namespace Ludus::Engine::Platform::Shell
{
	void Open(const std::filesystem::path& application, std::string_view args)
	{
		Ludus::Engine::Platform::Windows::Detail::ShellOpen(application, args);
	}
}
