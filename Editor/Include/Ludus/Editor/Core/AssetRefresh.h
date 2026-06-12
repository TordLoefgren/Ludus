#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <vector>

#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Core
{
	class ProjectSessionPersistence;

	enum class AssetRefreshClassification
	{
		Registered,
		Candidate,
		MissingSource,
		Unsupported
	};

	struct AssetRefreshEntry
	{
		std::filesystem::path Path;
		std::filesystem::path ManifestPath;
		Ludus::Engine::Core::AssetId Id { Ludus::Engine::Core::AssetId::Invalid() };
		Ludus::Engine::Core::AssetType Type { Ludus::Engine::Core::AssetType::Unknown };
		AssetRefreshClassification Classification { AssetRefreshClassification::Unsupported };
	};

	struct AssetRefreshSummary
	{
		std::vector<AssetRefreshEntry> Entries;
		std::size_t RegisteredCount = 0;
		std::size_t CandidateCount = 0;
		std::size_t MissingSourceCount = 0;
		std::size_t UnsupportedCount = 0;
	};

	std::optional<AssetRefreshEntry> TryClassifyAssetFile(
		const ProjectSessionPersistence& persistence,
		const std::filesystem::path& path
	);
}
