#pragma once

#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Graphics/Shader.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Graphics
{
	struct RenderView2D
	{
		Camera2D Camera;
		std::shared_ptr<RenderTarget> Target;
		Ludus::Engine::Math::Vector2D ViewportPosition;
		Ludus::Engine::Math::Vector2D ViewportSize;
	};
}
