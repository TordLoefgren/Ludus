#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Ludus::Engine::FileSystem
{
	struct FileDeleteScope
	{
		std::filesystem::path Path;
		~FileDeleteScope() noexcept;
	};

	struct DirectoryDeleteScope
	{
		std::filesystem::path Path;
		~DirectoryDeleteScope() noexcept;
	};

	std::string GenerateUniqueName(std::string_view prefix, std::string_view suffix);

	std::filesystem::path CreateTempFilePath(const std::filesystem::path& path);
	void CommitTempFile(FileDeleteScope& temp, const std::filesystem::path& destination);

	std::vector<std::filesystem::path> GetFilePaths(const std::filesystem::path& path);
	std::vector<std::string> GetFileNames(const std::filesystem::path& path);

	bool ArePathsEqual(const std::filesystem::path& left, const std::filesystem::path& right);

	std::vector<std::byte> ReadAllBytes(const std::filesystem::path& path);
	std::string ReadAllText(const std::filesystem::path& path);

	void WriteAllBytes(const std::filesystem::path& path, std::span<const std::byte> data);
	void WriteAllText(const std::filesystem::path& path, std::string_view data);
}
