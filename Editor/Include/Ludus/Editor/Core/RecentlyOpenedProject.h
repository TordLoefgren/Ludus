#pragma once

#include <filesystem>
#include <string>

namespace Ludus::Editor::Core
{
	struct RecentlyOpenedProject
	{
		std::string DisplayName;
		std::filesystem::path Path;
	};
}
