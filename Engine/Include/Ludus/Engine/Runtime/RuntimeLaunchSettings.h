#pragma once

#include <Ludus/Engine/Math/Size.h>

namespace Ludus::Engine::Runtime
{
	struct RuntimeLaunchSettings
	{
		struct Window
		{
			int StartupWidth = 1920;
			int StartupHeight = 1080;
			bool IsResizeable = true;
			bool IsMaximized = false;

			bool operator==(const Window& other) const
			{
				return StartupWidth == other.StartupWidth &&
					StartupHeight == other.StartupHeight &&
					IsResizeable == other.IsResizeable &&
					IsMaximized == other.IsMaximized;
			}
		};

		struct Presentation
		{
			Ludus::Engine::Math::Size<int> InternalResolution = { 1920, 1080 };
			bool UseFixedRenderResolution = false;

			bool operator==(const Presentation& other) const
			{
				return InternalResolution == other.InternalResolution &&
					UseFixedRenderResolution == other.UseFixedRenderResolution;
			}
		};

		Window WindowSettings;
		Presentation PresentationSettings;

		bool operator==(const RuntimeLaunchSettings& other) const
		{
			return PresentationSettings == other.PresentationSettings &&
				WindowSettings == other.WindowSettings;
		}
	};
}
