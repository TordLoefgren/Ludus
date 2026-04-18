#pragma once

#include <utility>

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Core/EditorState.h>

namespace Ludus::Editor::Core
{
	class EditorShell
	{
	public:
		Ludus::Editor::Build::BuildManager Build;
		EditorState State;

		explicit EditorShell(Ludus::Editor::Build::BuildManager buildManager)
			: Build(std::move(buildManager)), State()
		{
			Build.Initialize();
		}

		EditorShell(const EditorShell&) = delete;
		EditorShell& operator=(const EditorShell&) = delete;
		EditorShell(EditorShell&&) = delete;
		EditorShell& operator=(EditorShell&&) = delete;
	};
}
