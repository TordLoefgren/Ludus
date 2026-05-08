#pragma once

#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/IRenderPass.h>
#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/RenderPhaseOrder.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Graphics/TextureRegion.h>
#include <Ludus/Engine/Math/RectInt.h>

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

		TextureRegion ToTextureRegion(const Ludus::Engine::Math::RectInt& sourceRect, const Texture& texture)
		{
			if (sourceRect.IsEmpty())
			{
				return { };
			}

			const auto width = static_cast<float>(texture.Width());
			const auto height = static_cast<float>(texture.Height());

			return {
				.Left = static_cast<float>(sourceRect.X) / width,
				.Top = static_cast<float>(sourceRect.Y) / height,
				.Right = static_cast<float>(sourceRect.X + sourceRect.Width) / width,
				.Bottom = static_cast<float>(sourceRect.Y + sourceRect.Height) / height
			};
		}

		virtual bool Enabled(Ludus::Engine::Graphics::RenderContext2D& context) override
		{
			return context.RenderView.SceneId.has_value()
				&& context.RenderView.CameraSource != CameraSource::None;
		};

		virtual void Execute(Ludus::Engine::Graphics::RenderContext2D& context, Ludus::Engine::Graphics::Renderer2D& renderer) override
		{
			if (!context.RenderView.SceneId)
			{
				return;
			}

			const auto* scene = context.SceneRegistry.TryGetScene(*context.RenderView.SceneId);
			if (!scene)
			{
				return;
			}

			if (context.RenderView.CameraSource == Ludus::Engine::Graphics::CameraSource::None)
			{
				return;
			}

			const auto& ecs = scene->EntityComponentSystem;
			for (const auto& sprite : ecs.Sprites.View())
			{
				const auto* transform = ecs.Transforms.TryGetByOwner(sprite.OwnerId);
				if (!transform)
				{
					continue;
				}

				switch (sprite.Shape)
				{
					case Ludus::Engine::Graphics::Shape::Quad:
					{
						const auto textureResult = context.AssetManager.GetTexture2D(sprite.TextureId);
						if (textureResult.HasTexture())
						{
							TextureRegion region { };

							if (!textureResult.IsFallback)
							{
								region = ToTextureRegion(sprite.SourceRect, *textureResult.Texture);
							}

							renderer.DrawSprite(*transform, sprite.Color, textureResult.Texture, region, sprite.FlipX, sprite.FlipY);
						}
						else
						{
							renderer.DrawQuad(*transform, sprite.Color, nullptr, sprite.Fill);
						}

						break;
					}

					case Ludus::Engine::Graphics::Shape::Circle:
						renderer.DrawCircle(*transform, sprite.Color, sprite.Fill);
						break;
				}
			}

			for (const auto& text : ecs.Texts.View())
			{
				const auto* transform = ecs.Transforms.TryGetByOwner(text.OwnerId);
				if (transform)
				{
					renderer.DrawText(*transform, text.Text, text.Color, text.HorizontalTextAlignment);
				}
			}
		};
	};
}
