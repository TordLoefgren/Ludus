#pragma once

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Core/EditorState.h>

namespace Ludus::Editor::Core
{
	struct EditorShell
	{
		Ludus::Editor::Build::BuildManager Build;
		EditorState State;

		EditorShell()
			: Build(), State()
		{
			Build.Initialize();
		}

		EditorShell(const EditorShell&) = delete;
		EditorShell& operator=(const EditorShell&) = delete;
		EditorShell(EditorShell&&) = delete;
		EditorShell& operator=(EditorShell&&) = delete;
	};
}
