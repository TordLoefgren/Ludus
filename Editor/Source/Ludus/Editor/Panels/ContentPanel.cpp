#include "pch.h"

#include <optional>
#include <system_error>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/AssetRefresh.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ContentPanel.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/UI/Context/ScrollContext.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Panels
{
	namespace
	{
		namespace EditorCore = Ludus::Editor::Core;

		Ludus::UI::Elements::ContentBrowser::EntryPresentation CreateAssetEntryPresentation(
			const EditorCore::ProjectSessionPersistence& persistence,
			const Ludus::UI::Elements::ContentBrowser::EntryContext& entry
		)
		{
			Ludus::UI::Elements::ContentBrowser::EntryPresentation presentation;
			if (entry.IsDirectory)
			{
				return presentation;
			}

			const auto classification = EditorCore::TryClassifyAssetFile(persistence, entry.Path);
			if (!classification)
			{
				return presentation;
			}

			if (classification->Classification == EditorCore::AssetRefreshClassification::Candidate)
			{
				presentation.TextColor = Ludus::Engine::Graphics::Colors::Orange;
				presentation.Tooltip = "Not included in project";
			}
			else if (classification->Classification == EditorCore::AssetRefreshClassification::Unsupported)
			{
				presentation.TextColor = Ludus::Engine::Graphics::Colors::Gray;
				presentation.Tooltip = "Unsupported asset file";
			}

			return presentation;
		}
	}

	bool ContentPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::Flags::Panel | Ludus::UI::Flags::Window::HorizontalScrollbar;
		auto windowTitle = CreateWindowTitleWithIcon(ICON_FOLDER_CLOSED, "Content");

		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			std::optional<std::filesystem::path> pendingDeletePath;

			m_ContentBrowser.Update([&](const Ludus::UI::Elements::ContentBrowser::EntryContext& entry)
			{
				return CreateAssetEntryPresentation(context.ProjectSession.Persistence, entry);

			}, [&](const Ludus::UI::Elements::ContentBrowser::EntryContext& entry)
			{
				// Callback to populate the context menu for an entry in the content browser.
				const auto isSceneFile =
					!entry.IsDirectory &&
					Ludus::Engine::FileSystem::HasLogicalExtension(
						entry.Path,
						Ludus::Engine::Persistence::Paths::Constants::SceneExtension
					);
				const auto canOpen = isSceneFile;
				const auto canDelete = !entry.IsDirectory && !isSceneFile;

				if (Ludus::UI::Widgets::MenuItem("Open", nullptr, false, canOpen))
				{
					context.Shell.State.Commands.AddRequestCommand(
						Ludus::Editor::Commands::RequestCommand::OpenScene { entry.Path }
					);
				}

				if (Ludus::UI::Widgets::MenuItem("Delete", nullptr, false, canDelete))
				{
					pendingDeletePath = entry.Path;
				}
			}, [&](const Ludus::UI::Elements::ContentBrowser::DirectoryContext& directory)
			{
				// Callback to populate the background context menu for the current directory.
				(void)directory;
			});

			if (pendingDeletePath)
			{
				std::error_code errorCode;
				std::filesystem::remove(*pendingDeletePath, errorCode);
				if (!errorCode)
				{
					m_ContentBrowser.FromDirectory(context.ProjectSession.Persistence.GetProjectRoot());
				}
			}

			if (auto openedFilePath = m_ContentBrowser.ConsumeOpenedFilePath(); openedFilePath)
			{
				if (Ludus::Engine::FileSystem::HasLogicalExtension(
					*openedFilePath,
					Ludus::Engine::Persistence::Paths::Constants::SceneExtension
				))
				{
					context.Shell.State.Commands.AddRequestCommand(
						Ludus::Editor::Commands::RequestCommand::OpenScene { *openedFilePath }
					);
				}
			}
		}

		return true;
	}
}
