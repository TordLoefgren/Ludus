#pragma once

#include <Ludus/Engine/Math/Size.h>

namespace Ludus::Engine::Graphics
{
	struct RenderPresentationSettings
	{
		Ludus::Engine::Math::Size<int> InternalResolution = { 1920, 1080 };
		bool UseFixedRenderResolution = false;

		float GetAspectRatio(float fallback = 16.0f / 9.0f) const
		{
			if (InternalResolution.Width > 0 && InternalResolution.Height > 0)
			{
				return static_cast<float>(InternalResolution.Width) / static_cast<float>(InternalResolution.Height);
			}

			return fallback;
		}
	};
}
