#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>

namespace Ludus::Engine::Core
{
	struct RenderViewRegistry
	{
	private:
		std::vector<Ludus::Engine::Graphics::RenderView2D> m_RenderViews;

	public:
		std::span<const Ludus::Engine::Graphics::RenderView2D> View() const { return m_RenderViews; }

		void Register(const Ludus::Engine::Graphics::RenderView2D& renderView)
		{
			m_RenderViews.push_back(renderView);
		}

		void RegisterFullscreen(std::optional<SceneHandle> sceneHandle, const Ludus::Engine::Graphics::Camera2D& camera, const std::shared_ptr<Ludus::Engine::Graphics::RenderTarget>& target)
		{
			if (!target)
			{
				LUDUS_LOG_WARN("No render target to register with fullscreen.");
				return;
			}

			auto [width, height] = target->Framebuffer.GetSize();

			Ludus::Engine::Graphics::RenderView2D renderView {
				.Camera = camera,
				.SceneHandle = sceneHandle,
				.Target = target,
				.ViewportRect = Ludus::Engine::Math::Rect
				{
					{ 0.0f, 0.0f },
					{ static_cast<float>(width), static_cast<float>(height) }
				}
			};

			Register(renderView);
		}

		void Clear()
		{
			m_RenderViews.clear();
		}

		bool Empty() const { return m_RenderViews.empty(); }

		size_t GetSize() const { return m_RenderViews.size(); }
	};
}
