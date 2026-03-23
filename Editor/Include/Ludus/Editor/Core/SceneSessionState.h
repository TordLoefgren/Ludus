#pragma once

#include <filesystem>
#include <optional>

namespace Ludus::Editor::Core
{
	struct SceneSessionState
	{
		bool IsDirty = false;
		std::optional<std::filesystem::path> SavePath;
	};
}
