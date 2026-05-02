#pragma once

#include <memory>
#include <unordered_set>

#include <Ludus/Engine/Core/AssetRegistry.h>
#include <Ludus/Engine/Graphics/ITextureLoader.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Graphics/Texture2DResult.h>
#include <Ludus/Engine/Graphics/TextureCache.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Engine::Core
{
	class AssetManager
	{
	private:
		const AssetRegistry& m_AssetRegistry;
		std::filesystem::path m_RuntimeRootDirectory;
		std::unique_ptr<Ludus::Engine::Graphics::ITextureLoader> m_TextureLoader;
		Ludus::Engine::Graphics::TextureCache m_TextureCache;
		std::unordered_set<AssetId> m_FailedTextureLoads;
		Ludus::Engine::Graphics::Texture m_MissingTexture;

	public:
		AssetManager(
			const AssetRegistry& assetRegistry,
			std::filesystem::path runtimeRootDirectory
		);

		AssetManager(
			const AssetRegistry& assetRegistry,
			std::filesystem::path runtimeRootDirectory,
			std::unique_ptr<Ludus::Engine::Graphics::ITextureLoader> textureLoader,
			Ludus::Engine::Graphics::Texture missingTexture
		);

		Ludus::Engine::Graphics::Texture2DResult GetTexture2D(AssetId id);

		const Ludus::Engine::Runtime::AssetReference* TryGetReference(AssetId id) const;
		bool HasFailedTextureLoad(AssetId id) const;
	};
}
