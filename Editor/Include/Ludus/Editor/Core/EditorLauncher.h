#pragma once

#include <Ludus/Editor/Core/EditorStartupOptions.h>

namespace Ludus::Editor::Core
{
	struct EditorLauncher
	{
		static int Run(EditorStartupOptions editorStartupOptions);
	};
}
