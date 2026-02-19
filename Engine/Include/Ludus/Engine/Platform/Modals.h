#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace Ludus::Engine::Platform::Modals
{
	bool OpenFileDialog(
		std::string& filenameOut,
		std::string_view defaultExtension,
		std::optional<std::filesystem::path> defaultStartupPath = std::nullopt
	);

	bool SaveFileDialog(
		std::string& filenameOut,
		std::string_view defaultExtension,
		std::optional<std::filesystem::path> defaultStartupPath = std::nullopt,
		std::optional<std::string_view> defaultFileName = std::nullopt
	);
}
