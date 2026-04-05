#pragma once

#include <optional>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Rect.h>

namespace Ludus::Engine::Graphics
{
	struct RenderView2D
	{
		Ludus::Engine::Graphics::Camera2D Camera;
		std::optional<Ludus::Engine::Core::SceneId> SceneId;
		Ludus::Engine::Graphics::RenderTarget* Target = nullptr;
		Ludus::Engine::Math::Rect ViewportRect;
		Ludus::Engine::Graphics::CameraSource CameraSource = Ludus::Engine::Graphics::CameraSource::None;
	};
}
