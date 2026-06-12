#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Core
{
	struct AssetRefreshSummary;

	class ProjectSessionPersistence
	{
	private:
		Ludus::Editor::Core::ProjectManifest m_ProjectManifest;
		Ludus::Engine::Runtime::RuntimeManifest m_RuntimeManifest;
		Ludus::Engine::Runtime::RuntimeLaunchSettings m_RuntimeLaunchSettings;

		ProjectSessionPersistence(
			Ludus::Editor::Core::ProjectManifest projectManifest,
			Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
			Ludus::Engine::Runtime::RuntimeLaunchSettings runtimeLaunchSettings
		);

	public:
		static ProjectSessionPersistence Create(
			Ludus::Editor::Core::ProjectManifest projectManifest,
			Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
			Ludus::Engine::Runtime::RuntimeLaunchSettings runtimeLaunchSettings
		);

		const Ludus::Editor::Core::ProjectManifest& GetProjectManifest() const { return m_ProjectManifest; }
		const Ludus::Engine::Runtime::RuntimeManifest& GetRuntimeManifest() const { return m_RuntimeManifest; }
		const Ludus::Engine::Runtime::RuntimeLaunchSettings& GetRuntimeLaunchSettings() const { return m_RuntimeLaunchSettings; }

		const std::filesystem::path& GetProjectRoot() const { return m_ProjectManifest.ProjectRoot; }
		const std::filesystem::path& GetRuntimeManifestPath() const { return m_ProjectManifest.RuntimeManifestPath; }
		const std::vector<Ludus::Engine::Runtime::AssetReference>& GetAssets() const { return m_RuntimeManifest.Assets; }
		const std::vector<Ludus::Engine::Runtime::ScriptReference>& GetScripts() const { return m_RuntimeManifest.Scripts; }

		const std::string GetProjectName() const;
		const std::filesystem::path GetProjectManifestPath() const;
		const std::filesystem::path GetRuntimeLaunchSettingsPath() const;

		bool UpdateRuntimeLaunchSettings(const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings);

#pragma region Assets

		const Ludus::Engine::Runtime::AssetReference* TryGetAssetReference(const std::filesystem::path& path) const;
		bool AddOrUpdateAssetReference(Ludus::Engine::Core::AssetId id, Ludus::Engine::Core::AssetType type, std::filesystem::path path);
		bool IncludeAsset(Ludus::Engine::Core::AssetType type, std::filesystem::path path);
		bool RemoveAssetReference(Ludus::Engine::Core::AssetId id);
		bool HasAssetReference(Ludus::Engine::Core::AssetId id) const;
		bool HasAssetReference(const std::filesystem::path& path) const;
		Ludus::Engine::Core::AssetId AllocateAssetId() const;
		AssetRefreshSummary RefreshAssets() const;

#pragma endregion

#pragma region Scenes

		std::optional<std::filesystem::path> TryGetScenePath(Ludus::Engine::Core::SceneId id) const;
		bool AddOrUpdateSceneReference(Ludus::Engine::Core::SceneId id, std::string name, std::filesystem::path path);

#pragma endregion

#pragma region Scripts

		bool AddOrUpdateScriptReference(Ludus::Engine::Core::ScriptId id, std::string name);
		bool RemoveScriptReference(Ludus::Engine::Core::ScriptId id);
		bool HasScriptReference(Ludus::Engine::Core::ScriptId id) const;
		bool HasScriptReference(std::string_view name) const;

		Ludus::Engine::Core::ScriptId AllocateScriptId() const;
		const std::vector<std::string> GetScriptNames() const;

#pragma endregion

	};
}
