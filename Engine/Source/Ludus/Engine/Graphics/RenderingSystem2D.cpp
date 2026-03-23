#include "pch.h"

#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/RenderingSystem2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>

namespace Ludus::Engine::Graphics
{
	RenderingSystem2D::RenderingSystem2D(
		RenderingOptions renderingOptions,
		RenderingConfiguration2D& renderingConfiguration,
		Ludus::Engine::Core::RenderViewRegistry& renderViewRegistry,
		Ludus::Engine::Core::SceneRegistry& sceneRegistry
	) :
		m_RenderingOptions(renderingOptions),
		m_RenderingConfiguration(renderingConfiguration),
		m_RenderViewRegistry(renderViewRegistry),
		m_SceneRegistry(sceneRegistry),
		m_Shader(),
		m_Renderer(m_Shader),
		m_RenderingPipeline(m_RenderingConfiguration.GetRenderPasses())
	{
		m_Renderer.SetClearColor(m_RenderingOptions.ClearColor);
	}

	void RenderingSystem2D::RenderImpl()
	{
		for (auto& renderView : m_RenderViewRegistry.View())
		{
			auto* targetPtr = renderView.Target;
			if (!targetPtr)
			{
				LUDUS_LOG_ERROR("Render view has no render target.");
				return;
			}

			m_Renderer.SetClearColor(
				renderView.CameraSource == Ludus::Engine::Graphics::CameraSource::None ? Colors::Black : m_RenderingOptions.ClearColor
			);

			targetPtr->Framebuffer.Bind();

			m_Renderer.BeginScene(renderView.Camera);
			m_Renderer.Clear();

			m_RenderingPipeline.Execute({ m_SceneRegistry, renderView }, m_Renderer);

			m_Renderer.EndScene();

			targetPtr->Framebuffer.Unbind();
		}
	}
}
