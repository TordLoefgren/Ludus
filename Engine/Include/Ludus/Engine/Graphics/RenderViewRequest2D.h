#pragma once

#include <optional>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Graphics
{
	struct RenderViewRequest2D
	{
		std::optional<Ludus::Engine::Graphics::Camera2D> Camera;
		std::optional<Ludus::Engine::Core::SceneId> SceneId;
		RenderTarget* Target = nullptr;
		Ludus::Engine::Math::Rect ViewportRect;
	};
}
