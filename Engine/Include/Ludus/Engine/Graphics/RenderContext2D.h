#pragma once

#include <Ludus/Engine/Core/AssetManager.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>

namespace Ludus::Engine::Graphics
{
	struct RenderContext2D
	{
		Ludus::Engine::Core::AssetManager& AssetManager;
		Ludus::Engine::Core::SceneRegistry& SceneRegistry;
		const RenderView2D& RenderView;
	};
}
