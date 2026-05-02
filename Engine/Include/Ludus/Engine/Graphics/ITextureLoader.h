#pragma once

#include <filesystem>
#include <optional>

#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	class ITextureLoader
	{
	public:
		virtual ~ITextureLoader() = default;

		virtual std::optional<Texture> TryLoadTextureFromFile(const std::filesystem::path& path) const = 0;
	};
}
