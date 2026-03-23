#pragma once

#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>

namespace Ludus::Engine::Graphics
{
	struct RenderContext2D
	{
		Ludus::Engine::Core::SceneRegistry& SceneRegistry;
		const Ludus::Engine::Graphics::RenderView2D& RenderView;
		void* UserData = nullptr;
	};
}
