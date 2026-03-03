#pragma once

#include <filesystem>
#include <string_view>

namespace Ludus::Engine::Platform::Shell
{
	void Open(const std::filesystem::path& application, std::string_view args);
}
