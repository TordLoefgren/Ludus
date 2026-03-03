#pragma once

#include <Ludus/Editor/Build/BuildManager.h>
#include <Ludus/Editor/Core/EditorSession.h>
#include <Ludus/Editor/Core/EditorState.h>

namespace Ludus::Editor::Core
{
	struct EditorContext
	{
		Ludus::Editor::Build::BuildManager Build;
		EditorSession Session;
		EditorState State;

		EditorContext()
			: Build(), Session(), State()
		{
			Build.Initialize();
		}

		EditorContext(const EditorContext&) = delete;
		EditorContext& operator=(const EditorContext&) = delete;
		EditorContext(EditorContext&&) = delete;
		EditorContext& operator=(EditorContext&&) = delete;
	};
}
