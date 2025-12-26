#pragma once

namespace Ludus::Editor::Core
{
	struct EditorRequests
	{
		bool AddViewport = false;

		void Clear()
		{
			AddViewport = false;
		}
	};
}
