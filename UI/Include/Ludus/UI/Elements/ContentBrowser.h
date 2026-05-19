#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <Ludus/Engine/Graphics/Color.h>

namespace Ludus::UI::Elements
{
	struct ContentBrowser
	{
		struct Entry
		{
			std::string Name;
			std::filesystem::path Path;
			bool IsDirectory = false;
			std::vector<Entry> Children;
		};

		struct EntryContext
		{
			const std::filesystem::path& Path;
			bool IsDirectory = false;
			bool IsSelected = false;
		};

		struct EntryPresentation
		{
			const char* Icon = nullptr;
			std::optional<Ludus::Engine::Graphics::Color> TextColor;
			std::optional<std::string> Tooltip;
			bool IsEnabled = true;
		};

		struct DirectoryContext
		{
			const std::filesystem::path& DirectoryPath;
		};

		using PresentationCallback = std::function<EntryPresentation(const EntryContext&)>;
		using ContextMenuCallback = std::function<void(const EntryContext&)>;
		using BackgroundContextMenuCallback = std::function<void(const DirectoryContext&)>;

		bool Open = true;

		ContentBrowser();

		void Update(
			const PresentationCallback& presentationCallback = { },
			const ContextMenuCallback& contextMenuCallback = { },
			const BackgroundContextMenuCallback& backgroundContextMenuCallback = { }
		);

		void FromDirectory(const std::filesystem::path& directory);
		Entry BuildTree(const std::filesystem::path& directory);

		void Clear();
		std::optional<std::filesystem::path> ConsumeOpenedFilePath();

	private:
		std::vector<Entry> m_RootEntries;
		std::filesystem::path m_CurrentDirectoryPath;
		std::filesystem::path m_SelectedPath;

		std::optional<std::filesystem::path> m_PendingOpenedFilePath;
		bool m_SyncTreeExpansionToCurrentDirectory = false;

		const std::vector<Entry>* TryGetDirectoryEntries(const std::filesystem::path& path) const;

		bool IsSelected(const std::filesystem::path& path) const;
		bool IsCurrentDirectoryAncestorPath(const std::filesystem::path& path) const;

		void DrawDirectoryTree(const std::vector<Entry>& entries);
		void DrawDirectoryContents(
			const std::vector<Entry>& entries,
			const PresentationCallback& presentationCallback,
			const ContextMenuCallback& contextMenuCallback
		);
	};
}
