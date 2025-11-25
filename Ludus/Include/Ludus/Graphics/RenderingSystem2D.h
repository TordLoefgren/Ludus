#pragma once

#include <memory>

#include <Ludus/Core/SystemContext.h>
#include <Ludus/Engine/EntityComponentSystem.h>
#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Graphics/Shape.h>

namespace Ludus::Graphics
{
	class RenderingSystem2D final : public Ludus::Core::ISystem
	{
		std::unique_ptr<Camera2D> m_Camera;
		std::unique_ptr<Renderer2D> m_Renderer;
		std::unique_ptr<Shader> m_Shader;

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
			auto& windowOptions = m_SystemContext->Window.GetOptions();
			m_Camera->SetViewport(windowOptions.Width, windowOptions.Height);

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
			auto& ecs = m_SystemContext->EntityComponentSystem;

			m_Renderer->BeginScene(*m_Camera);
			m_Renderer->Clear();

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
		}
	};
}
