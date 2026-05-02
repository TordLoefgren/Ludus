#include "pch.h"

#include <Ludus/Engine/Graphics/TextureCache.h>

namespace Ludus::Engine::Graphics
{
	Texture* TextureCache::TryGet(Ludus::Engine::Core::AssetId id)
	{
		if (auto iter = m_TexturesByAssetId.find(id); iter != m_TexturesByAssetId.end())
		{
			return iter->second.get();
		}

		return nullptr;
	}

	Texture& TextureCache::Add(Ludus::Engine::Core::AssetId id, std::unique_ptr<Texture> texture)
	{
		LUDUS_ASSERT(texture != nullptr, "Cannot add a null texture in the texture cache.");
		LUDUS_ASSERT(!Contains(id), "Cannot add a duplicate texture asset id in the texture cache.");

		m_TexturesByAssetId[id] = std::move(texture);

		return *m_TexturesByAssetId.at(id);
	}

	bool TextureCache::Contains(Ludus::Engine::Core::AssetId id) const
	{
		return m_TexturesByAssetId.contains(id);
	}
}
