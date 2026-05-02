#pragma once

#include <memory>
#include <unordered_map>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	class TextureCache
	{
	private:
		std::unordered_map<Ludus::Engine::Core::AssetId, std::unique_ptr<Texture>> m_TexturesByAssetId;

	public:
		Texture* TryGet(Ludus::Engine::Core::AssetId id);
		Texture& Add(Ludus::Engine::Core::AssetId id, std::unique_ptr<Texture> texture);
		bool Contains(Ludus::Engine::Core::AssetId id) const;
	};
}
