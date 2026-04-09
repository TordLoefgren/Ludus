#pragma once

namespace Ludus::Editor::Core
{
	struct UnsavedChangesState
	{
		bool IsSceneDirty = false;
		bool IsProjectManifestDirty = false;
		bool IsRuntimeManifestDirty = false;
		bool IsRuntimeLaunchSettingsDirty = false;

		bool HasUnsavedChanges() const
		{
			return IsSceneDirty
				|| IsProjectManifestDirty
				|| IsRuntimeManifestDirty
				|| IsRuntimeLaunchSettingsDirty;
		}
	};
}
