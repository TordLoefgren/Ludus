#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <Ludus/Engine/Graphics/IRenderPass.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>

namespace Ludus::Engine::Graphics
{
	struct RenderingPipeline2D
	{
	private:
		const std::vector<std::unique_ptr<Ludus::Engine::Graphics::IRenderPass>>& m_RenderPasses;

	public:
		RenderingPipeline2D(const std::vector<std::unique_ptr<IRenderPass>>& renderPasses)
			: m_RenderPasses(renderPasses)
		{ }

		~RenderingPipeline2D() = default;

		void Execute(Ludus::Engine::Graphics::RenderContext2D renderContext, Ludus::Engine::Graphics::Renderer2D& renderer)
		{
			for (auto& pass : m_RenderPasses)
			{
				if (pass->Enabled(renderContext))
				{
					pass->Execute(renderContext, renderer);
				}
			}
		}
	};
}
