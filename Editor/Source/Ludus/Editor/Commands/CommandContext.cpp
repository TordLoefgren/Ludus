#include "pch.h"

#include <string>
#include <string_view>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Core/EditorMode.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Windowing/Window.h>

namespace Ludus::Editor::Commands
{
	void CommandContext::SetEditorMode(Ludus::Editor::Core::EditorMode mode, std::string_view windowTitleOverride)
	{
		EditorContext.State.Mode = mode;

		if (!windowTitleOverride.empty())
		{
			SystemContext.Window.SetTitle(windowTitleOverride);
			return;
		}

		if (mode == Ludus::Editor::Core::EditorMode::Startup)
		{
			SystemContext.Window.SetTitle("Ludus Editor");
			return;
		}

		if (SystemContext.HasProjectContext())
		{
			SystemContext.Window.SetTitle(
				SystemContext.ProjectContext.value().ProjectRootDirectory.filename().string() + " - Ludus Editor"
			);
			return;
		}

		SystemContext.Window.SetTitle("Ludus Editor");
	}
}
