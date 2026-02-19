#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Ludus::UI
{
	struct AssetBrowser
	{
		struct Entry
		{
			std::string Name;
			std::filesystem::path Path;
			bool IsDirectory = false;
			std::vector<Entry> Children;
		};

		bool Open = true;

		AssetBrowser();

		void Update();
		void FromDirectory(const std::filesystem::path& directory);
		Entry BuildTree(const std::filesystem::path& directory);
		void Clear();
		std::optional<std::filesystem::path> ConsumeOpenedFilePath();

	private:
		std::vector<Entry> m_RootEntries;
		std::vector<int> m_CurrentDirectoryPath;
		std::vector<int> m_SelectedPath;
		std::optional<std::filesystem::path> m_PendingOpenedFilePath;
		bool m_SyncTreeExpansionToCurrentDirectory = false;

		std::vector<Entry>* TryGetDirectoryEntries(std::vector<int> path);
		const std::vector<Entry>* TryGetDirectoryEntries(std::vector<int> path) const;
		std::optional<std::filesystem::path> TryGetPathFromIndices(const std::vector<int>& path) const;
		std::optional<std::vector<int>> TryFindIndicesFromPath(const std::filesystem::path& path) const;
		bool IsSelected(const std::vector<int>& path) const;
		bool IsCurrentDirectoryAncestorPath(const std::vector<int>& path) const;
		void DrawDirectoryTree(const std::vector<Entry>& entries, std::vector<int>& workingPath);
		void DrawDirectoryContents(std::vector<Entry>& entries, const std::vector<int>& directoryPath);
	};
}
