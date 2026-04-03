#include "pch.h"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <ranges>

#include <Ludus/UI/Context/ImageContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/SelectionContext.h>
#include <Ludus/UI/Elements/ContentBrowser.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::UI::Elements
{
	namespace
	{
		bool CompareEntries(const ContentBrowser::Entry& left, const ContentBrowser::Entry& right)
		{
			if (left.IsDirectory != right.IsDirectory)
			{
				return left.IsDirectory > right.IsDirectory;
			}

			return left.Name < right.Name;
		}
	}

	ContentBrowser::ContentBrowser()
		: m_RootEntries(), m_CurrentDirectoryPath(), m_SelectedPath(), m_PendingOpenedFilePath()
	{ }

	void ContentBrowser::FromDirectory(const std::filesystem::path& directory)
	{
		if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
		{
			Clear();
			return;
		}

		const auto previousCurrentDirectoryPath = TryGetPathFromIndices(m_CurrentDirectoryPath);
		const auto previousSelectedPath = TryGetPathFromIndices(m_SelectedPath);

		m_RootEntries.clear();
		m_PendingOpenedFilePath.reset();
		m_SyncTreeExpansionToCurrentDirectory = false;

		m_RootEntries.push_back(BuildTree(directory));

		if (previousCurrentDirectoryPath)
		{
			if (const auto restoredCurrentDirectoryPath = TryFindIndicesFromPath(*previousCurrentDirectoryPath); restoredCurrentDirectoryPath)
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

		if (previousSelectedPath)
		{
			if (const auto restoredSelectedPath = TryFindIndicesFromPath(*previousSelectedPath); restoredSelectedPath)
			{
				m_SelectedPath = *restoredSelectedPath;
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

	ContentBrowser::Entry ContentBrowser::BuildTree(const std::filesystem::path& directory)
	{
		ContentBrowser::Entry root;
		root.Name = directory.filename().string();
		root.Path = directory;
		root.IsDirectory = true;

		const auto options = std::filesystem::directory_options::skip_permission_denied;
		for (const auto& entry : std::filesystem::directory_iterator(directory, options))
		{
			ContentBrowser::Entry item;
			item.Name = entry.path().filename().string();
			item.Path = entry.path();
			item.IsDirectory = entry.is_directory();

			if (item.IsDirectory)
			{
				item = BuildTree(entry.path());
			}

			root.Children.push_back(std::move(item));
		}

		std::ranges::sort(root.Children, CompareEntries);
		return root;
	}

	void ContentBrowser::Clear()
	{
		m_RootEntries.clear();
		m_CurrentDirectoryPath.clear();
		m_SelectedPath.clear();
		m_PendingOpenedFilePath.reset();
		m_SyncTreeExpansionToCurrentDirectory = false;
	}

	std::optional<std::filesystem::path> ContentBrowser::ConsumeOpenedFilePath()
	{
		auto value = std::move(m_PendingOpenedFilePath);
		m_PendingOpenedFilePath.reset();
		return value;
	}

	void ContentBrowser::Update()
	{
		ImGui::BeginChild("##ContentBrowserTree", ImVec2(260.0f, 0.0f), ImGuiChildFlags_Borders);
		{
			std::vector<int> workingPath;
			DrawDirectoryTree(m_RootEntries, workingPath);
		}
		ImGui::EndChild();
		m_SyncTreeExpansionToCurrentDirectory = false;

		Ludus::UI::Context::LayoutContext::SameLine();

		ImGui::BeginChild("##ContentBrowserContent", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);
		{
			if (auto* entries = TryGetDirectoryEntries(m_CurrentDirectoryPath))
			{
				DrawDirectoryContents(*entries, m_CurrentDirectoryPath);
			}
		}
		ImGui::EndChild();
	}

	std::vector<ContentBrowser::Entry>* ContentBrowser::TryGetDirectoryEntries(std::vector<int> path)
	{
		auto* current = &m_RootEntries;

		for (const auto index : path)
		{
			if (index < 0 || index >= static_cast<int>(current->size()))
			{
				return nullptr;
			}

			auto& entry = current->at(static_cast<size_t>(index));
			if (!entry.IsDirectory)
			{
				return nullptr;
			}

			current = &entry.Children;
		}

		return current;
	}

	const std::vector<ContentBrowser::Entry>* ContentBrowser::TryGetDirectoryEntries(std::vector<int> path) const
	{
		auto* current = &m_RootEntries;

		for (const auto index : path)
		{
			if (index < 0 || index >= static_cast<int>(current->size()))
			{
				return nullptr;
			}

			const auto& entry = current->at(static_cast<size_t>(index));
			if (!entry.IsDirectory)
			{
				return nullptr;
			}

			current = &entry.Children;
		}

		return current;
	}

	std::optional<std::filesystem::path> ContentBrowser::TryGetPathFromIndices(const std::vector<int>& path) const
	{
		if (path.empty())
		{
			return std::nullopt;
		}

		const auto* current = &m_RootEntries;
		const Entry* resolvedEntry = nullptr;

		for (const auto index : path)
		{
			if (index < 0 || index >= static_cast<int>(current->size()))
			{
				return std::nullopt;
			}

			const auto& entry = current->at(static_cast<size_t>(index));
			resolvedEntry = &entry;
			current = &entry.Children;
		}

		if (resolvedEntry == nullptr)
		{
			return std::nullopt;
		}

		return resolvedEntry->Path;
	}

	std::optional<std::vector<int>> ContentBrowser::TryFindIndicesFromPath(const std::filesystem::path& path) const
	{
		if (path.empty())
		{
			return std::nullopt;
		}

		auto normalizePath = [](std::filesystem::path value)
		{
			value = std::filesystem::absolute(value).lexically_normal();
			value.make_preferred();
			return value;
		};

		const auto targetPath = normalizePath(path);
		std::vector<int> currentIndices;

		std::function<bool(const std::vector<Entry>&)> findPath = [&](const std::vector<Entry>& entries)
		{
			for (size_t i = 0; i < entries.size(); i++)
			{
				currentIndices.push_back(static_cast<int>(i));
				const auto& entry = entries[i];
				const auto entryPath = normalizePath(entry.Path);

				std::error_code errorCode;
				if (entryPath == targetPath || std::filesystem::equivalent(entryPath, targetPath, errorCode))
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

	bool ContentBrowser::IsSelected(const std::vector<int>& path) const
	{
		return m_SelectedPath == path;
	}

	bool ContentBrowser::IsCurrentDirectoryAncestorPath(const std::vector<int>& path) const
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

	void ContentBrowser::DrawDirectoryTree(const std::vector<Entry>& entries, std::vector<int>& workingPath)
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
			const auto flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (isLeaf ? (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen) : 0)
				| (workingPath == m_CurrentDirectoryPath ? ImGuiTreeNodeFlags_Selected : 0);

			const bool opened = ImGui::TreeNodeEx(entry.Path.generic_string().c_str(), flags, "%s %s", ICON_FOLDER_OPEN, entry.Name.c_str());
			if (ImGui::IsItemClicked())
			{
				m_CurrentDirectoryPath = workingPath;
			}

			if (opened && !isLeaf)
			{
				DrawDirectoryTree(entry.Children, workingPath);
				ImGui::TreePop();
			}

			workingPath.pop_back();
		}
	}

	void ContentBrowser::DrawDirectoryContents(std::vector<Entry>& entries, const std::vector<int>& directoryPath)
	{
		std::vector<int> itemPath = directoryPath;

		for (size_t i = 0; i < entries.size(); i++)
		{
			auto& entry = entries[i];
			itemPath.push_back(static_cast<int>(i));

			const auto isSelected = IsSelected(itemPath);
			const auto icon = entry.IsDirectory ? ICON_FOLDER_CLOSED : ICON_FILE;
			const auto label = std::string(icon) + " " + entry.Name + "##" + entry.Path.generic_string();
			if (Ludus::UI::Widgets::Selectable(label.c_str(), isSelected))
			{
				m_SelectedPath = itemPath;
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.IsDirectory)
				{
					m_CurrentDirectoryPath = itemPath;
					m_SyncTreeExpansionToCurrentDirectory = true;
				}
				else
				{
					m_PendingOpenedFilePath = entry.Path;
				}
			}

			itemPath.pop_back();
		}
	}
}
