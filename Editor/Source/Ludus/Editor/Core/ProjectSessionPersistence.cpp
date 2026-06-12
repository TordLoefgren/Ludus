#include <pch.h>

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <Ludus/Editor/Core/AssetRefresh.h>
#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Core/ProjectSessionPersistence.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Core
{
	namespace FileSystem = Ludus::Engine::FileSystem;

	namespace
	{
		void AddRefreshEntry(AssetRefreshSummary& summary, AssetRefreshEntry entry)
		{
			switch (entry.Classification)
			{
				case AssetRefreshClassification::Registered:
					++summary.RegisteredCount;
					break;
				case AssetRefreshClassification::Candidate:
					++summary.CandidateCount;
					break;
				case AssetRefreshClassification::MissingSource:
					++summary.MissingSourceCount;
					break;
				case AssetRefreshClassification::Unsupported:
					++summary.UnsupportedCount;
					break;
				default:
					throw std::runtime_error("Unsupported asset refresh classification.");
			}

			summary.Entries.push_back(std::move(entry));
		}
		std::optional<Ludus::Engine::Core::AssetType> TryGetSupportedAssetType(const std::filesystem::path& path)
		{
			using Ludus::Engine::Core::AssetType;

			if (FileSystem::HasLogicalExtension(path, ".png")
				|| FileSystem::HasLogicalExtension(path, ".jpg")
				|| FileSystem::HasLogicalExtension(path, ".jpeg")
				|| FileSystem::HasLogicalExtension(path, ".bmp"))
			{
				return AssetType::Texture2D;
			}

			return std::nullopt;
		}

	}

	std::optional<AssetRefreshEntry> TryClassifyAssetFile(
		const ProjectSessionPersistence& persistence,
		const std::filesystem::path& path
	)
	{
		const auto manifestPath = Ludus::Engine::Persistence::Paths::NormalizeRuntimeAssetPathOrEmpty(
			persistence.GetProjectRoot(),
			path
		);
		if (manifestPath.empty())
		{
			return std::nullopt;
		}

		if (const auto* assetReference = persistence.TryGetAssetReference(path))
		{
			return AssetRefreshEntry {
				.Path = path,
				.ManifestPath = manifestPath,
				.Id = assetReference->Id,
				.Type = assetReference->Type,
				.Classification = AssetRefreshClassification::Registered
			};
		}

		if (FileSystem::HasLogicalExtension(path, ".ludus"))
		{
			return std::nullopt;
		}

		const auto supportedType = TryGetSupportedAssetType(path);

		return AssetRefreshEntry {
			.Path = path,
			.ManifestPath = manifestPath,
			.Type = supportedType.value_or(Ludus::Engine::Core::AssetType::Unknown),
			.Classification = supportedType ? AssetRefreshClassification::Candidate : AssetRefreshClassification::Unsupported
		};
	}

	ProjectSessionPersistence::ProjectSessionPersistence(
		Ludus::Editor::Core::ProjectManifest projectManifest,
		Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
		Ludus::Engine::Runtime::RuntimeLaunchSettings runtimeLaunchSettings
	) :
		m_ProjectManifest(std::move(projectManifest)),
		m_RuntimeManifest(std::move(runtimeManifest)),
		m_RuntimeLaunchSettings(std::move(runtimeLaunchSettings))
	{}

	ProjectSessionPersistence ProjectSessionPersistence::Create(
		Ludus::Editor::Core::ProjectManifest projectManifest,
		Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
		Ludus::Engine::Runtime::RuntimeLaunchSettings runtimeLaunchSettings
	)
	{
		return { std::move(projectManifest), std::move(runtimeManifest), std::move(runtimeLaunchSettings) };
	}

	const std::string ProjectSessionPersistence::GetProjectName() const
	{
		// Remove ".runtime.ludus" to get the project name.
		return m_ProjectManifest.RuntimeManifestPath.stem().stem().string();
	}

	const std::filesystem::path ProjectSessionPersistence::GetProjectManifestPath() const
	{
		return Ludus::Editor::Persistence::ProjectPaths::ProjectManifestFile(
			GetProjectRoot(),
			GetProjectName()
		);
	}

	const std::filesystem::path ProjectSessionPersistence::GetRuntimeLaunchSettingsPath() const
	{
		return Ludus::Engine::Persistence::Paths::RuntimeLaunchSettingsFile(
			GetProjectRoot(),
			GetProjectName()
		);
	}

	bool ProjectSessionPersistence::UpdateRuntimeLaunchSettings(
		const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings
	)
	{
		if (runtimeLaunchSettings == m_RuntimeLaunchSettings)
		{
			return false;
		}

		m_RuntimeLaunchSettings = runtimeLaunchSettings;

		return true;
	}

#pragma region Assets

	bool ProjectSessionPersistence::AddOrUpdateAssetReference(
		Ludus::Engine::Core::AssetId id,
		Ludus::Engine::Core::AssetType type,
		std::filesystem::path path
	)
	{
		if (!id.IsValid())
		{
			throw std::runtime_error("Cannot register an asset with an invalid id.");
		}

		if (Ludus::Engine::Core::BuiltInAssetIds::IsBuiltIn(id))
		{
			throw std::runtime_error("Cannot register a built-in asset id in the runtime manifest.");
		}

		if (type == Ludus::Engine::Core::AssetType::Unknown)
		{
			throw std::runtime_error("Cannot register an asset with unknown asset type.");
		}

		path = Ludus::Engine::Persistence::Paths::NormalizeRuntimeAssetPathOrEmpty(
			GetProjectRoot(),
			std::move(path)
		);
		if (path.empty())
		{
			throw std::runtime_error("Cannot register an asset with an empty path.");
		}

		for (auto& assetReference : m_RuntimeManifest.Assets)
		{
			if (assetReference.Id == id)
			{
				if (assetReference.Type == type && FileSystem::NormalizePortablePath(assetReference.Path) == path)
				{
					return false;
				}

				assetReference.Type = type;
				assetReference.Path = std::move(path);

				return true;
			}
		}

		m_RuntimeManifest.Assets.push_back({ id, type, std::move(path) });

		return true;
	}

	bool ProjectSessionPersistence::RemoveAssetReference(Ludus::Engine::Core::AssetId id)
	{
		auto& assets = m_RuntimeManifest.Assets;
		for (auto iter = assets.begin(); iter != assets.end(); ++iter)
		{
			if (iter->Id == id)
			{
				assets.erase(iter);
				return true;
			}
		}

		return false;
	}

	bool ProjectSessionPersistence::HasAssetReference(Ludus::Engine::Core::AssetId id) const
	{
		for (const auto& assetReference : m_RuntimeManifest.Assets)
		{
			if (assetReference.Id == id)
			{
				return true;
			}
		}

		return false;
	}

	const Ludus::Engine::Runtime::AssetReference* ProjectSessionPersistence::TryGetAssetReference(const std::filesystem::path& path) const
	{
		const auto normalizedPath = Ludus::Engine::Persistence::Paths::NormalizeRuntimeAssetPathOrEmpty(
			GetProjectRoot(),
			path
		);
		if (normalizedPath.empty())
		{
			return nullptr;
		}

		for (const auto& assetReference : m_RuntimeManifest.Assets)
		{
			if (FileSystem::NormalizePortablePath(assetReference.Path) == normalizedPath)
			{
				return &assetReference;
			}
		}

		return nullptr;
	}

	bool ProjectSessionPersistence::HasAssetReference(const std::filesystem::path& path) const
	{
		return TryGetAssetReference(path) != nullptr;
	}

	Ludus::Engine::Core::AssetId ProjectSessionPersistence::AllocateAssetId() const
	{
		auto random = Ludus::Engine::Core::Random();
		auto id = Ludus::Engine::Core::AssetId { random.NextId() };

		while (HasAssetReference(id) || Ludus::Engine::Core::BuiltInAssetIds::IsBuiltIn(id))
		{
			id = Ludus::Engine::Core::AssetId { random.NextId() };
		}

		return id;
	}

	AssetRefreshSummary ProjectSessionPersistence::RefreshAssets() const
	{
		AssetRefreshSummary summary;
		const auto& projectRoot = GetProjectRoot();
		const auto assetsDirectory = Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot);
		const auto options = std::filesystem::directory_options::skip_permission_denied;
		std::unordered_set<std::string> existingManifestPaths;

		if (std::filesystem::exists(assetsDirectory) && std::filesystem::is_directory(assetsDirectory))
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDirectory, options))
			{
				if (!entry.is_regular_file())
				{
					continue;
				}

				auto classification = TryClassifyAssetFile(*this, entry.path());
				if (!classification)
				{
					continue;
				}

				if (classification->Classification == AssetRefreshClassification::Registered)
				{
					existingManifestPaths.insert(FileSystem::NormalizePortablePath(classification->ManifestPath).generic_string());
				}

				AddRefreshEntry(summary, std::move(*classification));
			}
		}

		for (const auto& assetReference : m_RuntimeManifest.Assets)
		{
			const auto manifestPath = FileSystem::NormalizePortablePath(assetReference.Path);
			if (existingManifestPaths.contains(manifestPath.generic_string()))
			{
				continue;
			}

			AddRefreshEntry(
				summary,
				{
					.Path = FileSystem::ResolvePathFromRoot(projectRoot, manifestPath),
					.ManifestPath = manifestPath,
					.Id = assetReference.Id,
					.Type = assetReference.Type,
					.Classification = AssetRefreshClassification::MissingSource
				}
			);
		}

		return summary;
	}

#pragma endregion

#pragma region Scenes

	std::optional<std::filesystem::path> ProjectSessionPersistence::TryGetScenePath(Ludus::Engine::Core::SceneId sceneId) const
	{
		std::filesystem::path scenePath;

		for (const auto& sceneReference : m_RuntimeManifest.Scenes)
		{
			if (sceneReference.Id == sceneId)
			{
				scenePath = sceneReference.Path;
				break;
			}
		}

		if (scenePath.empty())
		{
			return std::nullopt;
		}

		return scenePath;
	}

	bool ProjectSessionPersistence::AddOrUpdateSceneReference(
		Ludus::Engine::Core::SceneId id,
		std::string name,
		std::filesystem::path path
	)
	{
		path = Ludus::Engine::Persistence::Paths::NormalizeRuntimeScenePathOrEmpty(
			GetProjectRoot(),
			std::move(path)
		);
		if (path.empty())
		{
			throw std::runtime_error(
				"Cannot register a scene outside the project Scenes directory."
			);
		}

		for (auto& sceneReference : m_RuntimeManifest.Scenes)
		{
			if (sceneReference.Id == id)
			{
				if (sceneReference.Name == name && FileSystem::NormalizePortablePath(sceneReference.Path) == path)
				{
					return false;
				}

				sceneReference.Name = std::move(name);
				sceneReference.Path = std::move(path);

				return true;
			}
		}

		m_RuntimeManifest.Scenes.push_back({ id, std::move(name), std::move(path) });

		return true;
	}

#pragma endregion

#pragma region Scripts

	bool ProjectSessionPersistence::AddOrUpdateScriptReference(Ludus::Engine::Core::ScriptId id, std::string name)
	{
		for (auto& scriptReference : m_RuntimeManifest.Scripts)
		{
			if (scriptReference.Id == id)
			{
				if (scriptReference.Name == name)
				{
					return false;
				}

				scriptReference.Name = std::move(name);
				return true;
			}
		}

		m_RuntimeManifest.Scripts.push_back({ id, std::move(name) });
		return true;
	}

	bool ProjectSessionPersistence::RemoveScriptReference(Ludus::Engine::Core::ScriptId id)
	{
		auto& scripts = m_RuntimeManifest.Scripts;
		for (auto iter = scripts.begin(); iter != scripts.end(); ++iter)
		{
			if (iter->Id == id)
			{
				scripts.erase(iter);
				return true;
			}
		}

		return false;
	}

	bool ProjectSessionPersistence::HasScriptReference(Ludus::Engine::Core::ScriptId id) const
	{
		for (const auto& scriptReference : m_RuntimeManifest.Scripts)
		{
			if (scriptReference.Id == id)
			{
				return true;
			}
		}

		return false;
	}

	bool ProjectSessionPersistence::HasScriptReference(std::string_view name) const
	{
		for (const auto& scriptReference : m_RuntimeManifest.Scripts)
		{
			if (scriptReference.Name == name)
			{
				return true;
			}
		}

		return false;
	}

	Ludus::Engine::Core::ScriptId ProjectSessionPersistence::AllocateScriptId() const
	{
		auto random = Ludus::Engine::Core::Random();
		auto id = Ludus::Engine::Core::ScriptId { random.NextId() };

		while (HasScriptReference(id))
		{
			id = Ludus::Engine::Core::ScriptId { random.NextId() };
		}

		return id;
	}

	const std::vector<std::string> ProjectSessionPersistence::GetScriptNames() const
	{
		std::vector<std::string> names;
		names.reserve(m_RuntimeManifest.Scripts.size());

		for (const auto& scriptReference : m_RuntimeManifest.Scripts)
		{
			names.push_back(scriptReference.Name);
		}

		return names;
	}

#pragma endregion

}
