#pragma once

#include <filesystem>
#include <optional>
#include <utility>

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Core
{
	struct ActiveSceneSession
	{
		Ludus::Engine::Core::SceneId SceneId = Ludus::Engine::Core::SceneId::Invalid();
		std::optional<std::filesystem::path> SavePath;

		static ActiveSceneSession Create(
			Ludus::Engine::Core::SceneId id,
			std::optional<std::filesystem::path> savePath
		)
		{
			return { .SceneId = id, .SavePath = std::move(savePath) };
		}
	};
}
