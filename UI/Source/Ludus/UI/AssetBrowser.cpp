#include "pch.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>

#include <Ludus/UI/AssetBrowser.h>
#include <Ludus/UI/Icons/FontAwesome.h>

namespace Ludus::UI
{
	AssetBrowser::AssetBrowser()
		:m_RootEntries(), m_CurrentDirectoryPath(), m_SelectedPath()
	{ }

	void AssetBrowser::FromDirectory(const std::filesystem::path& directory)
	{
		const auto previousCurrentDirectoryPath = TryGetPathFromIndices(m_CurrentDirectoryPath);
		const auto previousSelectedPath = TryGetPathFromIndices(m_SelectedPath);
		m_RootEntries.clear();
		m_PendingOpenedFilePath.reset();
		m_SyncTreeExpansionToCurrentDirectory = false;

		auto root = BuildTree(directory);
		m_RootEntries.push_back(std::move(root));

		if (previousCurrentDirectoryPath.has_value())
		{
			if (const auto restoredCurrentDirectoryPath = TryFindIndicesFromPath(*previousCurrentDirectoryPath); restoredCurrentDirectoryPath.has_value())
			{
				m_CurrentDirectoryPath = *restoredCurrentDirectoryPath;
			}
			else
			{
				m_CurrentDirectoryPath.clear();
			}
		}
		else
		{
			m_CurrentDirectoryPath.clear();
		}

		if (previousSelectedPath.has_value())
		{
			if (const auto restoredSelectionPath = TryFindIndicesFromPath(*previousSelectedPath); restoredSelectionPath.has_value())
			{
				m_SelectedPath = *restoredSelectionPath;
			}
			else
			{
				m_SelectedPath.clear();
			}
		}
		else
		{
			m_SelectedPath.clear();
		}
	}

	AssetBrowser::Entry AssetBrowser::BuildTree(const std::filesystem::path& directory)
	{
		AssetBrowser::Entry root;
		root.Name = directory.filename().string();
		root.Path = directory;
		root.IsDirectory = true;

		const auto options = std::filesystem::directory_options::skip_permission_denied;
		for (const auto& iter : std::filesystem::directory_iterator(directory, options))
		{
			const auto& path = iter.path();

			Entry child;
			child.IsDirectory = iter.is_directory();
			child.Name = path.filename().string();
			child.Path = path;

			if (child.IsDirectory)
			{
				child = BuildTree(path);
				child.Name = path.filename().string();
			}

			root.Children.push_back(child);
		}

		return root;
	}

	void AssetBrowser::Clear()
	{
		m_RootEntries.clear();
		m_CurrentDirectoryPath.clear();
		m_SelectedPath.clear();
		m_PendingOpenedFilePath.reset();
		m_SyncTreeExpansionToCurrentDirectory = false;
	}

	std::optional<std::filesystem::path> AssetBrowser::ConsumeOpenedFilePath()
	{
		auto pendingPath = std::move(m_PendingOpenedFilePath);
		m_PendingOpenedFilePath.reset();
		return pendingPath;
	}

	void AssetBrowser::Update()
	{
		ImGui::BeginChild("##AssetBrowserTree", ImVec2(260.0f, 0.0f), ImGuiChildFlags_Borders);
		{
			std::vector<int> path;
			DrawDirectoryTree(m_RootEntries, path);
		}
		ImGui::EndChild();
		m_SyncTreeExpansionToCurrentDirectory = false;

		ImGui::SameLine();

		ImGui::BeginChild("##AssetBrowserContent", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);
		{
			if (auto* entries = TryGetDirectoryEntries(m_CurrentDirectoryPath))
			{
				DrawDirectoryContents(*entries, m_CurrentDirectoryPath);
			}
		}
		ImGui::EndChild();
	}

	std::vector<AssetBrowser::Entry>* AssetBrowser::TryGetDirectoryEntries(std::vector<int> path)
	{
		auto* entries = &m_RootEntries;
		for (int index : path)
		{
			if (index < 0 || static_cast<size_t>(index) >= entries->size())
			{
				return nullptr;
			}

			auto& entry = (*entries)[static_cast<size_t>(index)];
			if (!entry.IsDirectory)
			{
				return nullptr;
			}

			entries = &entry.Children;
		}

		return entries;
	}

	const std::vector<AssetBrowser::Entry>* AssetBrowser::TryGetDirectoryEntries(std::vector<int> path) const
	{
		auto* entries = &m_RootEntries;
		for (int index : path)
		{
			if (index < 0 || static_cast<size_t>(index) >= entries->size())
			{
				return nullptr;
			}

			const auto& entry = (*entries)[static_cast<size_t>(index)];
			if (!entry.IsDirectory)
			{
				return nullptr;
			}

			entries = &entry.Children;
		}

		return entries;
	}

	std::optional<std::filesystem::path> AssetBrowser::TryGetPathFromIndices(const std::vector<int>& path) const
	{
		if (path.empty())
		{
			return std::nullopt;
		}

		const auto* entries = &m_RootEntries;
		const Entry* resolvedEntry = nullptr;
		for (int index : path)
		{
			if (index < 0 || static_cast<size_t>(index) >= entries->size())
			{
				return std::nullopt;
			}

			resolvedEntry = &(*entries)[static_cast<size_t>(index)];
			entries = &resolvedEntry->Children;
		}

		if (resolvedEntry == nullptr)
		{
			return std::nullopt;
		}

		return resolvedEntry->Path;
	}

	std::optional<std::vector<int>> AssetBrowser::TryFindIndicesFromPath(const std::filesystem::path& path) const
	{
		auto normalizePath = [](std::filesystem::path value)
		{
			std::string asString = value.lexically_normal().make_preferred().string();
			std::transform(
				asString.begin(),
				asString.end(),
				asString.begin(),
				[](unsigned char character) { return static_cast<char>(std::tolower(character)); }
			);
			return asString;
		};

		const std::string targetPath = normalizePath(path);
		std::vector<int> currentIndices;

		std::function<bool(const std::vector<Entry>&)> findPath = [&](const std::vector<Entry>& entries)
		{
			for (size_t i = 0; i < entries.size(); i++)
			{
				currentIndices.push_back(static_cast<int>(i));
				const auto& entry = entries[i];
				if (normalizePath(entry.Path) == targetPath)
				{
					return true;
				}

				if (!entry.Children.empty() && findPath(entry.Children))
				{
					return true;
				}

				currentIndices.pop_back();
			}

			return false;
		};

		if (findPath(m_RootEntries))
		{
			return currentIndices;
		}

		return std::nullopt;
	}

	bool AssetBrowser::IsSelected(const std::vector<int>& path) const
	{
		return path == m_SelectedPath;
	}

	bool AssetBrowser::IsCurrentDirectoryAncestorPath(const std::vector<int>& path) const
	{
		if (path.empty() || path.size() > m_CurrentDirectoryPath.size())
		{
			return false;
		}

		for (size_t i = 0; i < path.size(); i++)
		{
			if (path[i] != m_CurrentDirectoryPath[i])
			{
				return false;
			}
		}

		return true;
	}

	void AssetBrowser::DrawDirectoryTree(const std::vector<Entry>& entries, std::vector<int>& workingPath)
	{
		for (size_t i = 0; i < entries.size(); i++)
		{
			const auto& entry = entries[i];
			if (!entry.IsDirectory)
			{
				continue;
			}

			workingPath.push_back(static_cast<int>(i));
			if (m_SyncTreeExpansionToCurrentDirectory && IsCurrentDirectoryAncestorPath(workingPath))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			}

			bool hasChildDirectories = false;
			for (const auto& child : entry.Children)
			{
				if (child.IsDirectory)
				{
					hasChildDirectories = true;
					break;
				}
			}
			const bool isLeaf = !hasChildDirectories;
			const auto treeFlags = ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (isLeaf ? (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen) : 0)
				| (workingPath == m_CurrentDirectoryPath ? ImGuiTreeNodeFlags_Selected : 0);
			const auto label = std::string(ICON_FOLDER_OPEN) + " " + entry.Name;
			const bool isOpen = ImGui::TreeNodeEx(label.c_str(), treeFlags);

			if (ImGui::IsItemClicked())
			{
				m_CurrentDirectoryPath = workingPath;
			}

			if (isOpen && !isLeaf)
			{
				DrawDirectoryTree(entry.Children, workingPath);
				ImGui::TreePop();
			}

			workingPath.pop_back();
		}
	}

	void AssetBrowser::DrawDirectoryContents(std::vector<Entry>& entries, const std::vector<int>& directoryPath)
	{
		for (size_t i = 0; i < entries.size(); i++)
		{
			const auto& entry = entries[i];

			auto entryPath = directoryPath;
			entryPath.push_back(static_cast<int>(i));

			const auto icon = entry.IsDirectory ? ICON_FOLDER : ICON_FILE;
			const auto label = std::string(icon) + " " + entry.Name + "##" + std::to_string(i);
			const bool selected = IsSelected(entryPath);
			if (ImGui::Selectable(label.c_str(), selected))
			{
				m_SelectedPath = entryPath;
			}

			if (entry.IsDirectory && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentDirectoryPath = entryPath;
				m_SyncTreeExpansionToCurrentDirectory = true;
			}
			else if (!entry.IsDirectory && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_PendingOpenedFilePath = entry.Path;
			}
		}
	}
}
