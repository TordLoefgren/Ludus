#pragma once

#include <filesystem>

namespace Ludus::Engine::Platform::Paths
{
	std::filesystem::path LocalAppData();

	std::filesystem::path RoamingAppData();

	void OpenFolder(const std::filesystem::path& path);
}
