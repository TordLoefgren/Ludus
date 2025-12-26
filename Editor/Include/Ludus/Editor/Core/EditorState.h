#pragma once

#include <Ludus/Editor/Core/EditorExecutionManager.h>
#include <Ludus/Editor/Core/EditorSelection.h>

namespace Ludus::Editor::Core
{
	struct EditorState
	{
		Ludus::Editor::Core::EditorExecutionManager ExecutionManager;
		Ludus::Editor::Core::EditorSelection Selection;
	};
}
