#pragma once

#include <cstdint>
#include <string>

#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderPhaseOrder.h>

namespace Ludus::Engine::Graphics
{
	struct IRenderPass
	{
		std::string Name;
		uint32_t Id = 0;
		Ludus::Engine::Graphics::RenderPhaseOrder Order = Ludus::Engine::Graphics::RenderPhaseOrder::Normal;

		virtual bool Enabled(Ludus::Engine::Graphics::RenderContext2D& context) = 0;
		virtual void Execute(Ludus::Engine::Graphics::RenderContext2D& context, Ludus::Engine::Graphics::Renderer2D& renderer) = 0;
	};
}
