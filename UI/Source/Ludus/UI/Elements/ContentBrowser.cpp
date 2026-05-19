#include "pch.h"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <ranges>

#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/UI/Context/ImageContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/SelectionContext.h>
#include <Ludus/UI/Elements/ContentBrowser.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Scope/PopupScope.h>
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

		const ContentBrowser::Entry* TryFindEntryByPath(
			const std::vector<ContentBrowser::Entry>& entries,
			const std::filesystem::path& path
		)
		{
			for (const auto& entry : entries)
			{
				if (Ludus::Engine::FileSystem::ArePathsEqual(entry.Path, path))
				{
					return &entry;
				}

				if (entry.IsDirectory)
				{
					if (const auto* found = TryFindEntryByPath(entry.Children, path))
					{
						return found;
					}
				}
			}

			return nullptr;
		}
	}

	ContentBrowser::ContentBrowser()
		: m_RootEntries(),
		m_CurrentDirectoryPath(),
		m_SelectedPath(),
		m_PendingOpenedFilePath()
	{}

	void ContentBrowser::FromDirectory(const std::filesystem::path& directory)
	{
		if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
		{
			Clear();
			return;
		}

		const auto previousCurrentDirectoryPath = m_CurrentDirectoryPath;
		const auto previousSelectedPath = m_SelectedPath;

		m_RootEntries.clear();
		m_CurrentDirectoryPath.clear();
		m_SelectedPath.clear();
		m_PendingOpenedFilePath.reset();
		m_SyncTreeExpansionToCurrentDirectory = false;

		m_RootEntries.push_back(BuildTree(directory));
		const auto rootPath = m_RootEntries.front().Path;

		if (!previousCurrentDirectoryPath.empty() && TryGetDirectoryEntries(previousCurrentDirectoryPath))
		{
			m_CurrentDirectoryPath = previousCurrentDirectoryPath;
		}
		else
		{
			m_CurrentDirectoryPath = rootPath;
		}

		if (!previousSelectedPath.empty() && TryFindEntryByPath(m_RootEntries, previousSelectedPath))
		{
			m_SelectedPath = previousSelectedPath;
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

	void ContentBrowser::Update(
		const PresentationCallback& presentationCallback,
		const ContextMenuCallback& contextMenuCallback,
		const BackgroundContextMenuCallback& backgroundContextMenuCallback
	)
	{
		ImGui::BeginChild("##ContentBrowserTree", ImVec2(260.0f, 0.0f), ImGuiChildFlags_Borders);
		{
			DrawDirectoryTree(m_RootEntries);
		}
		ImGui::EndChild();
		m_SyncTreeExpansionToCurrentDirectory = false;

		Ludus::UI::Context::LayoutContext::SameLine();

		ImGui::BeginChild("##ContentBrowserContent", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);
		{
			if (auto* entries = TryGetDirectoryEntries(m_CurrentDirectoryPath))
			{
				DrawDirectoryContents(*entries, presentationCallback, contextMenuCallback);
			}

			if (backgroundContextMenuCallback && !m_CurrentDirectoryPath.empty())
			{
				const auto popupFlags = Ludus::UI::Flags::Popup::MouseButtonRight | Ludus::UI::Flags::Popup::NoOpenOverItems;
				if (Ludus::UI::Scope::PopupContextWindowScope popup("##ContentBrowserBackgroundMenu", popupFlags); popup)
				{
					backgroundContextMenuCallback({ m_CurrentDirectoryPath });
				}
			}
		}
		ImGui::EndChild();
	}

	const std::vector<ContentBrowser::Entry>* ContentBrowser::TryGetDirectoryEntries(const std::filesystem::path& path) const
	{
		if (path.empty())
		{
			return &m_RootEntries;
		}

		auto* entry = TryFindEntryByPath(m_RootEntries, path);
		if (entry == nullptr || !entry->IsDirectory)
		{
			return nullptr;
		}

		return &entry->Children;
	}

	bool ContentBrowser::IsSelected(const std::filesystem::path& path) const
	{
		return !m_SelectedPath.empty() && Ludus::Engine::FileSystem::ArePathsEqual(m_SelectedPath, path);
	}

	bool ContentBrowser::IsCurrentDirectoryAncestorPath(const std::filesystem::path& path) const
	{
		return Ludus::Engine::FileSystem::IsAncestorPath(path, m_CurrentDirectoryPath);
	}

	void ContentBrowser::DrawDirectoryTree(const std::vector<Entry>& entries)
	{
		for (const auto& entry : entries)
		{
			if (!entry.IsDirectory)
			{
				continue;
			}

			if (m_SyncTreeExpansionToCurrentDirectory && IsCurrentDirectoryAncestorPath(entry.Path))
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
				| (Ludus::Engine::FileSystem::ArePathsEqual(entry.Path, m_CurrentDirectoryPath) ? ImGuiTreeNodeFlags_Selected : 0);

			const bool opened = ImGui::TreeNodeEx(entry.Path.generic_string().c_str(), flags, "%s %s", ICON_FOLDER_OPEN, entry.Name.c_str());
			if (ImGui::IsItemClicked())
			{
				m_CurrentDirectoryPath = entry.Path;
			}

			if (opened && !isLeaf)
			{
				DrawDirectoryTree(entry.Children);
				ImGui::TreePop();
			}
		}
	}

	void ContentBrowser::DrawDirectoryContents(
		const std::vector<Entry>& entries,
		const PresentationCallback& presentationCallback,
		const ContextMenuCallback& contextMenuCallback
	)
	{
		for (auto& entry : entries)
		{
			const auto isSelected = IsSelected(entry.Path);
			const EntryContext entryContext
			{
				.Path = entry.Path,
				.IsDirectory = entry.IsDirectory,
				.IsSelected = isSelected
			};

			auto presentation = EntryPresentation();
			if (presentationCallback)
			{
				presentation = presentationCallback(entryContext);
			}

			const auto icon = presentation.Icon != nullptr
				? presentation.Icon
				: (entry.IsDirectory ? ICON_FOLDER_CLOSED : ICON_FILE);
			const auto label = std::string(icon) + " " + entry.Name + "##" + entry.Path.generic_string();
			const auto textColor = presentation.TextColor.value_or(
				presentation.IsEnabled
				? Ludus::UI::Scope::GetStyleColor(Ludus::UI::Scope::Color::Text)
				: Ludus::UI::Scope::GetStyleColor(Ludus::UI::Scope::Color::TextDisabled)
			);
			{
				Ludus::UI::Scope::StyleColorScope textColorScope(
					{ Ludus::UI::Scope::Color::Text, textColor }
				);

				if (!presentation.IsEnabled)
				{
					ImGui::BeginDisabled();
				}

				if (Ludus::UI::Widgets::Selectable(label.c_str(), isSelected) && presentation.IsEnabled)
				{
					m_SelectedPath = entry.Path;
				}

				if (!presentation.IsEnabled)
				{
					ImGui::EndDisabled();
				}
			}

			if (presentation.IsEnabled && ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				m_SelectedPath = entry.Path;
			}

			if (presentation.Tooltip && ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("%s", presentation.Tooltip->c_str());
			}

			if (presentation.IsEnabled && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.IsDirectory)
				{
					m_CurrentDirectoryPath = entry.Path;
					m_SyncTreeExpansionToCurrentDirectory = true;
				}
				else
				{
					m_PendingOpenedFilePath = entry.Path;
				}
			}

			if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
			{
				if (contextMenuCallback)
				{
					contextMenuCallback(entryContext);
				}
			}
		}
	}
}
