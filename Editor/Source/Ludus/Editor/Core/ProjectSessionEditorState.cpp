#include <pch.h>

#include <filesystem>
#include <optional>
#include <utility>

#include <Ludus/Editor/Core/ProjectSessionEditorState.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Core
{
	ProjectSessionEditorState::ProjectSessionEditorState(
		ActiveSceneSession activeSceneSession
	) :
		m_ActiveScene(std::move(activeSceneSession)),
		m_Selection(),
		m_UnsavedChanges()
	{ }

	ProjectSessionEditorState ProjectSessionEditorState::Create(
		Ludus::Engine::Core::SceneId id,
		std::optional<std::filesystem::path> savePath
	)
	{
		return { ActiveSceneSession::Create(id, std::move(savePath)) };
	}

	void ProjectSessionEditorState::SetActiveScene(
		Ludus::Engine::Core::SceneId id,
		std::optional<std::filesystem::path> savePath
	)
	{
		m_ActiveScene = ActiveSceneSession::Create(id, savePath);
		m_UnsavedChanges.IsSceneDirty = false;
	}

	void ProjectSessionEditorState::SetActiveSceneSavePath(std::filesystem::path savePath)
	{
		m_ActiveScene.SavePath = std::move(savePath);
	}

	const std::filesystem::path& ProjectSessionEditorState::GetActiveSceneSavePath()
	{
		LUDUS_ASSERT(ActiveSceneHasSavePath(), "The active scene does not have a save path.");

		return *m_ActiveScene.SavePath;
	}

	Ludus::Engine::Core::SceneId ProjectSessionEditorState::GetActiveSceneId() const
	{
		return m_ActiveScene.SceneId;
	}

	bool ProjectSessionEditorState::ActiveSceneHasSavePath() const
	{
		return m_ActiveScene.SavePath.has_value();
	}

	void ProjectSessionEditorState::SetSceneDirty(bool flag)
	{
		m_UnsavedChanges.IsSceneDirty = flag;
	}

	void ProjectSessionEditorState::SetProjectManifestDirty(bool flag)
	{
		m_UnsavedChanges.IsProjectManifestDirty = flag;
	}

	void ProjectSessionEditorState::SetRuntimeManifestDirty(bool flag)
	{
		m_UnsavedChanges.IsRuntimeManifestDirty = flag;
	}

	void ProjectSessionEditorState::SetRuntimeLaunchSettingsDirty(bool flag)
	{
		m_UnsavedChanges.IsRuntimeLaunchSettingsDirty = flag;
	}
}
