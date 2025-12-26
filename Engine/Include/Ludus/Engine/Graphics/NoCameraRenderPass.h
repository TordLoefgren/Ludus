#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/IRenderPass.h>
#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderPhaseOrder.h>

namespace Ludus::Engine::Graphics
{
	struct NoCameraRenderPass final : public IRenderPass
	{
		NoCameraRenderPass()
		{
			Name = "No Camera Render Pass";
			Id = 999;
			Order = RenderPhaseOrder::After;
		}

		virtual bool Enabled(RenderContext2D& context) override
		{
			if (!context.RenderView.SceneHandle.has_value())
			{
				return false;
			}

			return context.RenderView.CameraSource == Ludus::Engine::Graphics::CameraSource::None;
		}

		virtual void Execute(RenderContext2D& context, Renderer2D& renderer) override
		{
			const auto worldRect = context.RenderView.Camera.GetWorldRect();

			const Ludus::Engine::Components::Transform2DComponent transform(
				0,
				{ worldRect.Position.X, worldRect.Position.Y },
				{ 0.03f, 0.03f },
				0.0f
			);

			renderer.DrawText(
				transform,
				"No Camera",
				Ludus::Engine::Graphics::Colors::White,
				Ludus::Engine::Graphics::HorizontalTextAlignment::Center
			);
		}
	};
}
