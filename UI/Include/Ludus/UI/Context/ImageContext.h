#pragma once

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Context::ImageContext
{
	enum class ImageUV
	{
		Default,
		FlipY
	};

	void CreateImage(
		uint32_t textureHandle,
		Ludus::Engine::Math::Vector2D imageSize = { 0.0f, 0.0f },
		ImageUV imageUV = ImageUV::FlipY
	);
}
