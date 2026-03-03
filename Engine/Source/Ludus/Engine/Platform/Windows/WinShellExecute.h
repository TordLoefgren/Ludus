#pragma once

#include <filesystem>
#include <string_view>

namespace Ludus::Engine::Platform::Windows::Detail
{
	void ShellOpen(
		const std::filesystem::path& target,
		std::string_view parameters = {}
	);
}