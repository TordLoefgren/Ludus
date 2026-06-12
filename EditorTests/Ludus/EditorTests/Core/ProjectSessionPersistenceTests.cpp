#include "pch.h"

#include <filesystem>
#include <utility>
#include <vector>

#include <Ludus/Editor/Core/AssetRefresh.h>
#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Core/ProjectSessionPersistence.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::EditorTests::Core
{
	namespace EditorCore = Ludus::Editor::Core;
	namespace FileSystem = Ludus::Engine::FileSystem;
	namespace Runtime = Ludus::Engine::Runtime;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_Editor_ProjectSessionPersistence_Tests_", "");
	}

	static FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		const auto path = MakeUniqueTempDir();
		std::filesystem::create_directories(path);
		return FileSystem::DirectoryDeleteScope { path };
	}

	static EditorCore::ProjectSessionPersistence CreatePersistence(
		const std::filesystem::path& projectRoot,
		std::vector<Runtime::AssetReference> assets
	)
	{
		return EditorCore::ProjectSessionPersistence::Create(
			EditorCore::ProjectManifest::Create(projectRoot, projectRoot / "Sandbox.runtime.ludus"),
			Runtime::RuntimeManifest::Create(
				Ludus::Engine::Core::SceneId::Invalid(),
				{ },
				{ },
				std::move(assets)
			),
			Runtime::RuntimeLaunchSettings()
		);
	}

	static const EditorCore::AssetRefreshEntry* FindEntry(
		const EditorCore::AssetRefreshSummary& summary,
		EditorCore::AssetRefreshClassification classification,
		const std::filesystem::path& manifestPath
	)
	{
		for (const auto& entry : summary.Entries)
		{
			if (
				entry.Classification == classification
				&& FileSystem::NormalizePortablePath(entry.ManifestPath) == FileSystem::NormalizePortablePath(manifestPath)
				)
			{
				return &entry;
			}
		}

		return nullptr;
	}

	TEST(ProjectSessionPersistence, RefreshAssets_ClassifiesRegisteredCandidateMissingSourceAndUnsupportedFiles)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";
		const auto assetsDirectory = Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot);
		std::filesystem::create_directories(assetsDirectory);
		FileSystem::WriteAllText(assetsDirectory / "Registered.png", "");
		FileSystem::WriteAllText(assetsDirectory / "Candidate.png", "");
		FileSystem::WriteAllText(assetsDirectory / "Unsupported.txt", "");
		FileSystem::WriteAllText(assetsDirectory / "Sandbox.project.ludus", "");
		FileSystem::WriteAllText(assetsDirectory / "Sandbox.runtime.ludus", "");
		FileSystem::WriteAllText(assetsDirectory / "Sandbox.settings.ludus", "");
		FileSystem::WriteAllText(assetsDirectory / "Main.scene.ludus", "");
		FileSystem::WriteAllText(projectRoot / "Outside.png", "");
		FileSystem::WriteAllText(projectRoot / "Sandbox.runtime.ludus", "");
		std::filesystem::create_directories(projectRoot / "Scenes");
		FileSystem::WriteAllText(projectRoot / "Scenes" / "Main.scene.ludus", "");

		const auto persistence = CreatePersistence(projectRoot, {
			{
				Ludus::Engine::Core::AssetId { 1 },
				Ludus::Engine::Core::AssetType::Texture2D,
				std::filesystem::path("Assets") / "Registered.png"
			},
			{
				Ludus::Engine::Core::AssetId { 2 },
				Ludus::Engine::Core::AssetType::Texture2D,
				std::filesystem::path("Assets") / "Missing.png"
			}
			});

		// Act.
		const auto summary = persistence.RefreshAssets();

		// Assert.
		ASSERT_EQ(summary.RegisteredCount, 1);
		ASSERT_EQ(summary.CandidateCount, 1);
		ASSERT_EQ(summary.MissingSourceCount, 1);
		ASSERT_EQ(summary.UnsupportedCount, 1);
		ASSERT_NE(FindEntry(summary, EditorCore::AssetRefreshClassification::Registered, std::filesystem::path("Assets") / "Registered.png"), nullptr);
		ASSERT_NE(FindEntry(summary, EditorCore::AssetRefreshClassification::Candidate, std::filesystem::path("Assets") / "Candidate.png"), nullptr);
		ASSERT_NE(FindEntry(summary, EditorCore::AssetRefreshClassification::MissingSource, std::filesystem::path("Assets") / "Missing.png"), nullptr);
		ASSERT_NE(FindEntry(summary, EditorCore::AssetRefreshClassification::Unsupported, std::filesystem::path("Assets") / "Unsupported.txt"), nullptr);
		ASSERT_EQ(summary.Entries.size(), 4);
		ASSERT_EQ(persistence.GetAssets().size(), 2);
	}

	TEST(ProjectSessionPersistence, RefreshAssets_TreatsExternalRenameAsMissingSourceAndCandidate)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";
		const auto assetsDirectory = Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot);
		std::filesystem::create_directories(assetsDirectory);
		FileSystem::WriteAllText(assetsDirectory / "Renamed.png", "");

		const auto persistence = CreatePersistence(projectRoot, {
			{
				Ludus::Engine::Core::AssetId { 1 },
				Ludus::Engine::Core::AssetType::Texture2D,
				std::filesystem::path("Assets") / "Original.png"
			}
			});

		// Act.
		const auto summary = persistence.RefreshAssets();

		// Assert.
		ASSERT_EQ(summary.CandidateCount, 1);
		ASSERT_EQ(summary.MissingSourceCount, 1);
		ASSERT_NE(FindEntry(summary, EditorCore::AssetRefreshClassification::Candidate, std::filesystem::path("Assets") / "Renamed.png"), nullptr);
		ASSERT_NE(FindEntry(summary, EditorCore::AssetRefreshClassification::MissingSource, std::filesystem::path("Assets") / "Original.png"), nullptr);
		ASSERT_EQ(persistence.GetAssets().size(), 1);
	}

	TEST(ProjectSessionPersistence, RefreshAssets_IsSafeToRunRepeatedlyWithoutChangingManifestState)
	{
		// Arrange.
		const auto tempDirectoryScope = CreateTestDirectory();
		const auto projectRoot = tempDirectoryScope.Path / "Sandbox";
		const auto assetsDirectory = Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot);
		std::filesystem::create_directories(assetsDirectory);
		FileSystem::WriteAllText(assetsDirectory / "Candidate.png", "");

		const auto persistence = CreatePersistence(projectRoot, {
			{
				Ludus::Engine::Core::AssetId { 1 },
				Ludus::Engine::Core::AssetType::Texture2D,
				std::filesystem::path("Assets") / "Missing.png"
			}
			});

		// Act.
		const auto firstSummary = persistence.RefreshAssets();
		const auto secondSummary = persistence.RefreshAssets();

		// Assert.
		ASSERT_EQ(firstSummary.CandidateCount, 1);
		ASSERT_EQ(firstSummary.MissingSourceCount, 1);
		ASSERT_EQ(secondSummary.CandidateCount, 1);
		ASSERT_EQ(secondSummary.MissingSourceCount, 1);
		ASSERT_EQ(persistence.GetAssets().size(), 1);
		ASSERT_EQ(persistence.GetAssets().front().Path, std::filesystem::path("Assets") / "Missing.png");
	}
}
