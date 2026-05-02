#pragma once

#include <filesystem>
#include <optional>

#include <Ludus/Engine/Graphics/ITextureLoader.h>

namespace Ludus::Engine::Graphics
{
	class TextureLoader final : public ITextureLoader
	{
	public:
		std::optional<Texture> TryLoadTextureFromFile(const std::filesystem::path& path) const override
		{
			if (!std::filesystem::exists(path))
			{
				return std::nullopt;
			}

			return Texture::TryFromFile(path);
		}
	};
}
