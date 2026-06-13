#pragma once

#include <cstdint>
#include <filesystem>
#include <utility>

namespace Ludus::Editor::Core
{
	struct ProjectManifest
	{
		inline static constexpr std::uint32_t CurrentSchemaRevision = 1;

		std::uint32_t SchemaRevision = CurrentSchemaRevision;
		std::filesystem::path ProjectRoot;
		std::filesystem::path RuntimeManifestPath;

		static ProjectManifest Create(std::filesystem::path projectRoot, std::filesystem::path runtimeManifestPath)
		{
			return {
				.SchemaRevision = CurrentSchemaRevision,
				.ProjectRoot = std::move(projectRoot),
				.RuntimeManifestPath = std::move(runtimeManifestPath)
			};
		}
	};
}
