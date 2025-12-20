#pragma once

#include <memory>

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingPipeline2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/Shader.h>
#include <Ludus/Engine/Graphics/Shape.h>

namespace Ludus::Engine::Graphics
{
	class RenderingSystem2D final : public Ludus::Engine::Core::ISystem
	{
		std::unique_ptr<RenderingPipeline2D> m_RenderingPipeline;
		std::unique_ptr<Renderer2D> m_Renderer;
		std::unique_ptr<Shader> m_Shader;
		RenderingOptions m_RenderingOptions;
		RenderingConfiguration2D& m_RenderingConfiguration;

	public:
		RenderingSystem2D(RenderingOptions renderingOptions, RenderingConfiguration2D& renderingConfiguration)
			: m_RenderingPipeline(nullptr), m_Renderer(nullptr), m_RenderingOptions(renderingOptions), m_RenderingConfiguration(renderingConfiguration)
		{ }
		~RenderingSystem2D() = default;

	protected:
		virtual void OnAttachImpl() override
		{
			m_Shader = std::make_unique<Shader>();
			m_Renderer = std::make_unique<Renderer2D>(*m_Shader);
			m_Renderer->SetClearColor(m_RenderingOptions.ClearColor);
			m_RenderingPipeline = std::make_unique<RenderingPipeline2D>(m_RenderingConfiguration.GetRenderPasses());
		}

		virtual void RenderImpl() override
		{
			for (auto& renderView : m_SystemContext->RenderViews.View())
			{
				auto* targetPtr = renderView.Target.get();
				if (!targetPtr)
				{
					LUDUS_LOG_ERROR("Render view has no render target.");
					return;
				}

				// Execute render passes.
				targetPtr->Framebuffer.Bind();

				m_Renderer->BeginScene(renderView.Camera);
				m_Renderer->Clear();

				m_RenderingPipeline->Execute({ m_SystemContext, renderView }, *m_Renderer);

				m_Renderer->EndScene();

				targetPtr->Framebuffer.Unbind();
			}
		}
	};
}
