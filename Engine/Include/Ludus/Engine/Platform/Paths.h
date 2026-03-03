#pragma once

#include <filesystem>

namespace Ludus::Engine::Platform::Paths
{
	std::filesystem::path LocalAppData();

	std::filesystem::path RoamingAppData();

	std::filesystem::path GetExecutablePath();

	void OpenFolder(const std::filesystem::path& path);
}
