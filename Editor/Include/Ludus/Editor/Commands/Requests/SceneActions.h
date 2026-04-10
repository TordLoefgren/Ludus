#pragma once

#include <filesystem>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
	void CreateSceneAction(ProjectSessionCommandContext& context);
	void CreateSceneAsAction(const std::filesystem::path& path, ProjectSessionCommandContext& context);
	void OpenSceneAction(const std::filesystem::path& path, ProjectSessionCommandContext& context);
	void SaveSceneAction(Ludus::Engine::Core::SceneId sceneId, ProjectSessionCommandContext& context);
	void SaveSceneAsAction(
		Ludus::Engine::Core::SceneId sceneId,
		const std::filesystem::path& path,
		ProjectSessionCommandContext& context
	);
	void RenameSceneAction(
		Ludus::Engine::Core::SceneId sceneId,
		const std::filesystem::path& path,
		ProjectSessionCommandContext& context
	);
}
