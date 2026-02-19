#pragma once

#include <filesystem>

#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Core
{
	struct EditorSceneMetadata
	{
		std::optional<std::filesystem::path> Path;
		bool IsDirty = false;

		bool IsOpenInHierarchy = true;
		bool IsVisibleInViewports = true;
		bool IsEditableInViewports = true;
	};
}
