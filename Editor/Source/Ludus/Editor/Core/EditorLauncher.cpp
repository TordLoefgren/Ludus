#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Core/EditorHostBuilder.h>
#include <Ludus/Editor/Core/EditorLauncher.h>
#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/EditorStartupOptions.h>
#include <Ludus/Editor/Persistence/EditorPersistence.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>

namespace Ludus::Editor::Core
{
	int EditorLauncher::Run(EditorStartupOptions editorStartupOptions)
	{
		auto editorPreferences = EditorPreferences::Default();
		auto editorPersistence = Ludus::Editor::Persistence::EditorPersistence::DefaultText();

		const auto editorPreferencesPath = Ludus::Editor::Persistence::ProjectPaths::EditorPreferencesFile();
		if (std::filesystem::exists(editorPreferencesPath))
		{
			editorPreferences = editorPersistence.EditorPreferences().Load(editorPreferencesPath);
		}

		auto host = EditorHostBuilder::Create()
			.WithEditorStartupOptions(std::move(editorStartupOptions))
			.WithEditorPersistence(std::move(editorPersistence))
			.WithEditorPreferences(std::move(editorPreferences))
			.UseEditorHostDefaults()
			.UseEditor()
			.Build();

		host->Run();

		return 0;
	}
}
