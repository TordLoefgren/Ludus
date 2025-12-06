#pragma once

#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>

namespace Ludus::Engine::Graphics
{
	struct IRenderViewProvider
	{
		virtual bool BuildRenderView(const Ludus::Engine::Core::SystemContext& context, RenderView2D& outRenderView) = 0;
	};
}
