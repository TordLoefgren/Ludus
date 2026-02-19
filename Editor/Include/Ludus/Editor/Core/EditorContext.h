#pragma once

#include <Ludus/Editor/Core/EditorSession.h>
#include <Ludus/Editor/Core/EditorState.h>

namespace Ludus::Editor::Core
{
	struct EditorContext
	{
		Ludus::Editor::Core::EditorSession Session;
		EditorState State;

		EditorContext()
			: Session(), State()
		{ }

		EditorContext(const EditorContext&) = delete;
		EditorContext& operator=(const EditorContext&) = delete;
		EditorContext(EditorContext&&) = delete;
		EditorContext& operator=(EditorContext&&) = delete;
	};
}
