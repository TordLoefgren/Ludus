#pragma once

#include <memory>

#include <Ludus/Core/SystemContext.h>
#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/RenderTarget.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Graphics/Shape.h>

namespace Ludus::Graphics
{
	class RenderingSystem2D final : public Ludus::Core::ISystem
	{
		std::unique_ptr<Camera2D> m_Camera;
		std::unique_ptr<Renderer2D> m_Renderer;
		std::unique_ptr<Shader> m_Shader;

		int m_ViewportWidth = 0;
		int m_ViewportHeight = 0;

		RenderingOptions m_RenderingOptions;

	public:
		RenderingSystem2D(RenderingOptions renderingOptions)
			: m_Camera(nullptr), m_Shader(nullptr), m_Renderer(nullptr), m_RenderingOptions(renderingOptions)
		{ }
		~RenderingSystem2D() = default;

	protected:
		virtual void OnAttachImpl() override
		{
			m_Camera = std::make_unique<Camera2D>();
			auto [currentWidth, currentHeight] = m_SystemContext->Window.GetFramebufferSize();
			m_Camera->SetViewport(currentWidth, currentHeight);

			m_ViewportWidth = currentWidth;
			m_ViewportHeight = currentHeight;

			m_Shader = std::make_unique<Shader>("Resources/Shaders");
			m_Renderer = std::make_unique<Renderer2D>(*m_Shader);
			m_Renderer->SetClearColor(m_RenderingOptions.ClearColor);
		}

		virtual void OnDetachImpl() override
		{
			m_Shader->Unbind();
		}

		virtual void RenderImpl() override
		{
			auto* target = m_SystemContext->Resources.Get<std::shared_ptr<Ludus::Graphics::RenderTarget>>().get();
			if (!target)
			{
				LUDUS_LOG_ERROR("Render target was not available.");
				return;
			}

			// Resize logic.
			auto [currentWidth, currentHeight] = m_SystemContext->Window.GetFramebufferSize();
			if (currentWidth != m_ViewportWidth || currentHeight != m_ViewportHeight)
			{
				m_ViewportWidth = currentWidth;
				m_ViewportHeight = currentHeight;

				m_Camera->SetViewport(currentWidth, currentHeight);

				target->Framebuffer.Resize(currentWidth, currentHeight);
			}

			// Render pass.
			target->Framebuffer.Bind();

			m_Renderer->BeginScene(*m_Camera);
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
					case Ludus::Graphics::Shape::Rect:
						m_Renderer->DrawQuad(*transform, sprite.Color, sprite.Texture, sprite.Fill);
						break;
					case Ludus::Graphics::Shape::Circle:
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

			target->Framebuffer.Unbind();
		}
	};
}
