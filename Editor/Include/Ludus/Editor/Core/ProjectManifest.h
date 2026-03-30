#pragma once

#include <filesystem>
#include <utility>

#include <Ludus/Engine/Core/Version.h>

namespace Ludus::Editor::Core
{
	struct ProjectManifest
	{
		inline static constexpr Ludus::Engine::Core::Version CurrentVersion = { 0, 2, 0 };

		Ludus::Engine::Core::Version Version = CurrentVersion;
		std::filesystem::path ProjectRoot;
		std::filesystem::path RuntimeManifestPath;

		static ProjectManifest Create(std::filesystem::path projectRoot, std::filesystem::path runtimeManifestPath)
		{
			return {
				.Version = CurrentVersion,
				.ProjectRoot = std::move(projectRoot),
				.RuntimeManifestPath = std::move(runtimeManifestPath)
			};
		}
	};
}
