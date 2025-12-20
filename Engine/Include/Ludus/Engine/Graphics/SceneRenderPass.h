#pragma once

#include <Ludus/Engine/Graphics/IRenderPass.h>
#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderPhaseOrder.h>

namespace Ludus::Engine::Graphics
{
	struct SceneRenderPass final : public IRenderPass
	{
		SceneRenderPass()
		{
			Name = "Scene Render Pass";
			Id = 1;
			Order = RenderPhaseOrder::Normal;
		}

		virtual bool Enabled(Ludus::Engine::Graphics::RenderContext2D& context) override
		{
			return true;
		};

		virtual void Execute(Ludus::Engine::Graphics::RenderContext2D& context, Ludus::Engine::Graphics::Renderer2D& renderer) override
		{
			auto& ecs = context.SystemContext->EntityComponentSystem;

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
						renderer.DrawQuad(*transform, sprite.Color, sprite.Texture, sprite.Fill);
						break;
					case Ludus::Engine::Graphics::Shape::Circle:
						renderer.DrawCircle(*transform, sprite.Color, sprite.Fill);
						break;
				}
			}

			for (const auto& text : ecs.Texts.View())
			{
				const auto* transform = ecs.Transforms.TryGetByOwner(text.OwnerHandle);
				if (transform)
				{
					renderer.DrawText(*transform, text.Text, text.Color, text.HorizontalAlignment);
				}
			}
		};
	};
}
