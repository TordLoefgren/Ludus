#pragma once

#include <memory>

#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Graphics
{
	struct RenderView2D
	{
		Camera2D Camera;
		std::shared_ptr<RenderTarget> Target;
		Ludus::Engine::Math::Rect ViewportRect;
	};
}
