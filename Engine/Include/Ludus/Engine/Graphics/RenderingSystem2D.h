#pragma once

#include <memory>

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/Shader.h>
#include <Ludus/Engine/Graphics/Shape.h>

namespace Ludus::Engine::Graphics
{
	class RenderingSystem2D final : public Ludus::Engine::Core::ISystem
	{
		std::unique_ptr<Renderer2D> m_Renderer;
		std::unique_ptr<Shader> m_Shader;
		RenderingOptions m_RenderingOptions;

	public:
		RenderingSystem2D(RenderingOptions renderingOptions)
			: m_Renderer(nullptr), m_RenderingOptions(renderingOptions)
		{ }
		~RenderingSystem2D() = default;

	protected:
		virtual void OnAttachImpl() override
		{
			m_Shader = std::make_unique<Shader>();
			m_Renderer = std::make_unique<Renderer2D>(*m_Shader);
			m_Renderer->SetClearColor(m_RenderingOptions.ClearColor);
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

				// Render pass.
				targetPtr->Framebuffer.Bind();

				m_Renderer->BeginScene(renderView.Camera);
				m_Renderer->Clear();

				auto& ecs = m_SystemContext->EntityComponentSystem;

				for (const auto& sprite : ecs.Sprites.View())
				{
					const auto* transform = ecs.Transforms.TryGetByOwner(sprite.OwnerHandle);
					if (!transform)
					{
						continue;
					}

					switch (sprite.Shape)
					{
						case Ludus::Engine::Graphics::Shape::Rect:
							m_Renderer->DrawQuad(*transform, sprite.Color, sprite.Texture, sprite.Fill);
							break;
						case Ludus::Engine::Graphics::Shape::Circle:
							m_Renderer->DrawCircle(*transform, sprite.Color, sprite.Fill);
							break;
					}
				}

				for (const auto& text : ecs.Texts.View())
				{
					const auto* transform = ecs.Transforms.TryGetByOwner(text.OwnerHandle);
					if (transform)
					{
						m_Renderer->DrawText(*transform, text.Text, text.Color, text.HorizontalAlignment);
					}
				}

				m_Renderer->EndScene();

				targetPtr->Framebuffer.Unbind();
			}
		}
	};
}
