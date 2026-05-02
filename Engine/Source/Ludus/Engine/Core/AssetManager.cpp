#include "pch.h"

#include <filesystem>
#include <format>

#include <Ludus/Engine/Core/AssetManager.h>
#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Graphics/TextureLoader.h>

namespace Ludus::Engine::Core
{
	AssetManager::AssetManager(
		const AssetRegistry& assetRegistry,
		std::filesystem::path runtimeRootDirectory
	) :
		AssetManager(
			assetRegistry,
			std::move(runtimeRootDirectory),
			std::make_unique<Ludus::Engine::Graphics::TextureLoader>(),
			Ludus::Engine::Graphics::Texture::Missing()
		)
	{}

	AssetManager::AssetManager(
		const AssetRegistry& assetRegistry,
		std::filesystem::path runtimeRootDirectory,
		std::unique_ptr<Ludus::Engine::Graphics::ITextureLoader> textureLoader,
		Ludus::Engine::Graphics::Texture missingTexture
	) :
		m_AssetRegistry(assetRegistry),
		m_RuntimeRootDirectory(std::move(runtimeRootDirectory)),
		m_TextureLoader(std::move(textureLoader)),
		m_TextureCache(),
		m_FailedTextureLoads(),
		m_MissingTexture(std::move(missingTexture))
	{
		LUDUS_ASSERT(m_TextureLoader != nullptr, "AssetManager requires a texture loader.");
	}

	Ludus::Engine::Graphics::Texture2DResult AssetManager::GetTexture2D(AssetId id)
	{
		if (!id.IsValid())
		{
			return { nullptr, false };
		}

		auto* cachedTexture = m_TextureCache.TryGet(id);
		if (cachedTexture != nullptr)
		{
			return { cachedTexture, false };
		}

		if (HasFailedTextureLoad(id))
		{
			return { &m_MissingTexture, true };
		}

		auto* assetReference = TryGetReference(id);
		if (assetReference == nullptr)
		{
			LUDUS_LOG_WARN(std::format("Asset reference with ID {} missing.", id.Value));
			m_FailedTextureLoads.emplace(id);

			return { &m_MissingTexture, true };
		}

		if (assetReference->Type != AssetType::Texture2D)
		{
			LUDUS_LOG_WARN(std::format("Asset reference with ID {} is not a texture.", id.Value));
			m_FailedTextureLoads.emplace(id);

			return { &m_MissingTexture, true };
		}

		const auto& texturePath = m_RuntimeRootDirectory / assetReference->Path;
		if (!std::filesystem::exists(texturePath))
		{
			LUDUS_LOG_WARN(std::format("Asset reference with ID {} has an invalid path.", id.Value));
			m_FailedTextureLoads.emplace(id);

			return { &m_MissingTexture, true };
		}

		auto loadedTextureResult = m_TextureLoader->TryLoadTextureFromFile(texturePath);
		if (!loadedTextureResult)
		{
			LUDUS_LOG_WARN(std::format("Asset reference with ID {} failed to load its texture.", id.Value));
			m_FailedTextureLoads.emplace(id);

			return { &m_MissingTexture, true };
		}

		auto texture = std::make_unique<Ludus::Engine::Graphics::Texture>(std::move(*loadedTextureResult));
		auto& storedTexture = m_TextureCache.Add(id, std::move(texture));

		return { &storedTexture, false };
	}

	const Ludus::Engine::Runtime::AssetReference* AssetManager::TryGetReference(AssetId id) const
	{
		return m_AssetRegistry.TryGetAsset(id);
	}

	bool AssetManager::HasFailedTextureLoad(AssetId id) const
	{
		return m_FailedTextureLoads.contains(id);
	}
}
