#pragma once

#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	struct Texture2DResult
	{
		Texture* Texture = nullptr;
		bool IsFallback = false;

		bool HasTexture() const
		{
			return Texture != nullptr;
		}
	};
}
