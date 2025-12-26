#pragma once

#include <cmath>

#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/IRenderPass.h>
#include <Ludus/Engine/Graphics/RenderContext2D.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderPhaseOrder.h>

namespace Ludus::Editor::Core
{
	struct EditorGridRenderPass final : public Ludus::Engine::Graphics::IRenderPass
	{
	private:
		Ludus::Engine::Graphics::Color m_OuterGridColor = Ludus::Engine::Graphics::Colors::White.WithAlpha(0.20f);
		Ludus::Engine::Graphics::Color m_InnerGridColor = Ludus::Engine::Graphics::Colors::White.WithAlpha(0.10f);
		int m_Margin = 1;

	public:
		EditorGridRenderPass()
		{
			Name = "Editor Grid Render Pass";
			Id = 2;
			Order = Ludus::Engine::Graphics::RenderPhaseOrder::After;
		}

		virtual bool Enabled(Ludus::Engine::Graphics::RenderContext2D& context) override
		{
			return context.RenderView.CameraSource == Ludus::Engine::Graphics::CameraSource::Explicit;
		};

		virtual void Execute(Ludus::Engine::Graphics::RenderContext2D& context, Ludus::Engine::Graphics::Renderer2D& renderer) override
		{
			const auto worldRect = context.RenderView.Camera.GetWorldRect();

			const auto [xPosition, yPosition] = worldRect.Position;
			const auto [width, height] = worldRect.Size;

			const auto left = xPosition - width * 0.5f;
			const auto right = xPosition + width * 0.5f;
			const auto bottom = yPosition - height * 0.5f;
			const auto top = yPosition + height * 0.5f;

			const auto xStart = static_cast<int>(std::floor(left)) - m_Margin;
			const auto xStop = static_cast<int>(std::ceil(right)) + m_Margin;
			const auto yStart = static_cast<int>(std::floor(bottom)) - m_Margin;
			const auto yStop = static_cast<int>(std::ceil(top)) + m_Margin;

			for (int x = xStart; x <= xStop; x++)
			{
				renderer.DrawLine(
					static_cast<float>(x),
					static_cast<float>(yStart),
					static_cast<float>(x),
					static_cast<float>(yStop),
					x % 10 == 0 ? m_OuterGridColor : m_InnerGridColor
				);
			}

			for (int y = yStart; y <= yStop; y++)
			{
				renderer.DrawLine(
					static_cast<float>(xStart),
					static_cast<float>(y),
					static_cast<float>(xStop),
					static_cast<float>(y),
					y % 10 == 0 ? m_OuterGridColor : m_InnerGridColor
				);
			}
		};
	};
}
