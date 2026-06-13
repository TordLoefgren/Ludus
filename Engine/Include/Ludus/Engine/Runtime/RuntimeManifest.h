#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Runtime
{
	struct SceneReference
	{
		Ludus::Engine::Core::SceneId Id { Ludus::Engine::Core::SceneId::Invalid() };
		std::string Name;
		std::filesystem::path Path;
	};

	struct ScriptReference
	{
		Ludus::Engine::Core::ScriptId Id { Ludus::Engine::Core::ScriptId::Invalid() };
		std::string Name;
	};

	struct AssetReference
	{
		Ludus::Engine::Core::AssetId Id { Ludus::Engine::Core::AssetId::Invalid() };
		Ludus::Engine::Core::AssetType Type { Ludus::Engine::Core::AssetType::Unknown };
		std::filesystem::path Path;
	};

	struct RuntimeManifest
	{
		inline static constexpr std::uint32_t CurrentSchemaRevision = 1;

		std::uint32_t SchemaRevision = CurrentSchemaRevision;
		Ludus::Engine::Core::SceneId EntrySceneId { Ludus::Engine::Core::SceneId::Invalid() };
		std::vector<SceneReference> Scenes;
		std::vector<ScriptReference> Scripts;
		std::vector<AssetReference> Assets;

		static RuntimeManifest Create(
			Ludus::Engine::Core::SceneId entrySceneId = Ludus::Engine::Core::SceneId::Invalid(),
			std::vector<SceneReference> scenes = { },
			std::vector<ScriptReference> scripts = { },
			std::vector<AssetReference> assets = { }
		)
		{
			return {
				.SchemaRevision = CurrentSchemaRevision,
				.EntrySceneId = entrySceneId,
				.Scenes = std::move(scenes),
				.Scripts = std::move(scripts),
				.Assets = std::move(assets)
			};
		}

		const AssetReference* TryGetAssetReference(Ludus::Engine::Core::AssetId id) const
		{
			for (const auto& assetReference : Assets)
			{
				if (assetReference.Id == id)
				{
					return &assetReference;
				}
			}

			return nullptr;
		}

		const ScriptReference* TryGetScriptReference(Ludus::Engine::Core::ScriptId id) const
		{
			for (const auto& scriptReference : Scripts)
			{
				if (scriptReference.Id == id)
				{
					return &scriptReference;
				}
			}

			return nullptr;
		}
	};
}
