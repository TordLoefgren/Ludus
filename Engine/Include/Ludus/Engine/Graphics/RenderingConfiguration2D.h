#pragma once

#include <memory>
#include <vector>

#include <Ludus/Engine/Graphics/IRenderPass.h>
#include <Ludus/Engine/Graphics/NoCameraRenderPass.h>
#include <Ludus/Engine/Graphics/SceneRenderPass.h>

namespace Ludus::Engine::Graphics
{
	struct RenderingConfiguration2D
	{
	private:
		std::vector<std::unique_ptr<Ludus::Engine::Graphics::IRenderPass>> m_RenderPasses;

		void SortPasses()
		{
			std::sort(
				m_RenderPasses.begin(),
				m_RenderPasses.end(),
				[](const auto& a, const auto& b) { return a->Order < b->Order; }
			);
		}

	public:
		RenderingConfiguration2D(std::vector<std::unique_ptr<IRenderPass>> passes)
			: m_RenderPasses(std::move(passes))
		{
			SortPasses();
		}

		RenderingConfiguration2D()
			: m_RenderPasses()
		{
			m_RenderPasses.push_back(std::make_unique<SceneRenderPass>());
			m_RenderPasses.push_back(std::make_unique<NoCameraRenderPass>());

			SortPasses();
		}

		void AddPass(std::unique_ptr<Ludus::Engine::Graphics::IRenderPass> renderPass)
		{
			m_RenderPasses.push_back(std::move(renderPass));
		}

		const std::vector<std::unique_ptr<Ludus::Engine::Graphics::IRenderPass>>& GetRenderPasses() const
		{
			return m_RenderPasses;
		}
	};
}
