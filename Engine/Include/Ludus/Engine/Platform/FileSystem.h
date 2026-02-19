#pragma once

#include <filesystem>

namespace Ludus::Engine::Platform::FileSystem
{
	void ReplaceFile(const std::filesystem::path& temp, const std::filesystem::path& destination);
}
