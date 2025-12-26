#pragma once

#include <memory>
#include <optional>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Graphics
{
	struct RenderViewRequest2D
	{
		std::optional<Ludus::Engine::Graphics::Camera2D> Camera;
		std::optional<Ludus::Engine::Core::SceneHandle> SceneHandle;
		std::shared_ptr<RenderTarget> Target;
		Ludus::Engine::Math::Rect ViewportRect;
	};
}
