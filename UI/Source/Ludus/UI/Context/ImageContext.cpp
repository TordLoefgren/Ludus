#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/ImageContext.h>

namespace Ludus::UI::Context::ImageContext
{
	void CreateImage(
		uint32_t textureHandle,
		Ludus::Engine::Math::Vector2D imageSize,
		ImageUV imageUV
	)
	{
		ImGui::Image(
			(ImTextureID)(intptr_t)textureHandle,
			{ imageSize.X, imageSize.Y },
			imageUV == ImageUV::Default ? ImVec2(0.0f, 0.0f) : ImVec2(0.0f, 1.0f),
			imageUV == ImageUV::Default ? ImVec2(1.0f, 1.0f) : ImVec2(1.0f, 0.0f)
		);
	}
}
