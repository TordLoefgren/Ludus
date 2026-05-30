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

	std::vector<std::filesystem::path> GetFilePaths(const std::filesystem::path& path);
	std::vector<std::string> GetFileNames(const std::filesystem::path& path);

	std::string ReadAllText(const std::filesystem::path& path);
	std::vector<std::byte> ReadAllBytes(const std::filesystem::path& path);

	void WriteAllText(const std::filesystem::path& path, std::string_view text);
	void WriteAllBytes(const std::filesystem::path& path, std::span<const std::byte> data);

	void CopyFileOverwrite(const std::filesystem::path& source, const std::filesystem::path& destination);
	void RemoveIfExists(const std::filesystem::path& path);
	void ReplaceDirectory(const std::filesystem::path& source, const std::filesystem::path& destination);
	void RemoveDirectoryIfEmpty(const std::filesystem::path& path);

	std::filesystem::path ResolvePathFromRoot(const std::filesystem::path& root, const std::filesystem::path& relativePath);
	std::string ToPortablePathString(const std::filesystem::path& path);
	std::filesystem::path NormalizePortablePath(const std::filesystem::path& path);
	std::filesystem::path NormalizePathRelativeToRootOrEmpty(const std::filesystem::path& root, const std::filesystem::path& path);
	bool ArePathsEqual(const std::filesystem::path& left, const std::filesystem::path& right);
	bool IsAncestorPath(const std::filesystem::path& ancestor, const std::filesystem::path& path);
	bool IsRelativePathUnderDirectory(const std::filesystem::path& path, std::string_view directoryName);
	bool HasLogicalExtension(const std::filesystem::path& path, std::string_view extension);
}
