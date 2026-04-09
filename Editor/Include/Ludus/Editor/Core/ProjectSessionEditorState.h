#pragma once

#include <filesystem>
#include <optional>
#include <utility>

#include <Ludus/Editor/Core/ActiveSceneSession.h>
#include <Ludus/Editor/Core/SelectionManager.h>
#include <Ludus/Editor/Core/UnsavedChangesState.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Core
{
	class ProjectSessionEditorState
	{
	private:
		ActiveSceneSession m_ActiveScene;
		SelectionManager m_Selection;
		UnsavedChangesState m_UnsavedChanges;

		ProjectSessionEditorState(ActiveSceneSession activeSceneSession);

	public:
		static ProjectSessionEditorState Create(
			Ludus::Engine::Core::SceneId id,
			std::optional<std::filesystem::path> savePath = std::nullopt
		);

		SelectionManager& GetSelection() { return m_Selection; }
		const SelectionManager& GetSelection() const { return m_Selection; }

		bool IsSceneDirty() const { return m_UnsavedChanges.IsSceneDirty; }
		bool IsProjectManifestDirty() const { return m_UnsavedChanges.IsProjectManifestDirty; }
		bool IsRuntimeManifestDirty() const { return m_UnsavedChanges.IsRuntimeManifestDirty; }
		bool IsRuntimeLaunchSettingsDirty() const { return m_UnsavedChanges.IsRuntimeLaunchSettingsDirty; }
		bool HasUnsavedChanges() const { return m_UnsavedChanges.HasUnsavedChanges(); }

		void SetActiveScene(
			Ludus::Engine::Core::SceneId id,
			std::optional<std::filesystem::path> savePath = std::nullopt
		);
		void SetActiveSceneSavePath(std::filesystem::path savePath);
		const std::filesystem::path& GetActiveSceneSavePath();

		Ludus::Engine::Core::SceneId GetActiveSceneId() const;
		bool ActiveSceneHasSavePath() const;

		void SetSceneDirty(bool flag);
		void SetProjectManifestDirty(bool flag);
		void SetRuntimeManifestDirty(bool flag);
		void SetRuntimeLaunchSettingsDirty(bool flag);
	};
}
