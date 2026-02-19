#pragma once

#include <optional>

#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/RenderViewConfiguration.h>
#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>

namespace Ludus::Engine::Graphics
{
	class RenderViewSystem final : public Ludus::Engine::Core::ISystem
	{
	private:
		Ludus::Engine::Graphics::RenderViewConfiguration m_RenderViewConfiguration;

	public:
		RenderViewSystem(Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration = Ludus::Engine::Graphics::RenderViewConfiguration())
			: m_RenderViewConfiguration(renderViewConfiguration)
		{ }

		~RenderViewSystem() = default;

	protected:
		virtual void RenderImpl() override
		{
			// Pull render view requests.
			auto renderRequests = m_SystemContext->RenderViewRequests.View();
			if (renderRequests.empty())
			{
				// Check configuration.
				if (m_RenderViewConfiguration.EnableDefaultViewFallback)
				{
					const auto [framebufferWidth, framebufferHeight] = m_SystemContext->Window.GetFramebufferSize();
					auto camera = CreateDefaultCamera(framebufferWidth, framebufferHeight);

					m_SystemContext->RenderViews.RegisterFullscreen(std::nullopt, camera, m_SystemContext->WindowRenderTarget);
				}

				return;
			}

			// For each render view request, resolve it into an actual render view.
			for (const auto& request : renderRequests)
			{
				if (!request.Target)
				{
					LUDUS_LOG_WARN("No target available for render view request.");
					continue;
				}

				// Push render views.
				auto resolvedRequest = ResolveRequest(*m_SystemContext, request);
				m_SystemContext->RenderViews.Register(std::move(resolvedRequest));
			}
		}

		static Ludus::Engine::Graphics::Camera2D CreateDefaultCamera(int targetWidth, int targetHeight)
		{
			Camera2D camera;
			camera.SetViewport(targetWidth, targetHeight);
			camera.SetPosition({ 0,0 });
			camera.SetRotation(0);
			camera.SetOrthographicSize(10);

			return camera;
		}

		static RenderView2D ResolveRequest(Ludus::Engine::Core::SystemContext& context, const RenderViewRequest2D& request)
		{
			auto [targetWidth, targetHeight] = request.Target->Framebuffer.GetSize();

			// Explicit camera -> Tool camera.
			if (request.Camera.has_value())
			{
				auto camera = request.Camera.value();
				camera.SetViewport(targetWidth, targetHeight);

				return {
					.Camera = camera,
					.SceneHandle = request.SceneHandle,
					.Target = request.Target,
					.ViewportRect = request.ViewportRect,
					.CameraSource = Ludus::Engine::Graphics::CameraSource::Explicit
				};
			}

			// Resolve camera from scene -> Scene primary camera view.
			if (request.SceneHandle.has_value())
			{
				auto* scene = context.SceneRegistry.TryGetScene(request.SceneHandle.value());
				if (scene)
				{
					if (auto primaryCamera = scene->TryGetPrimaryCamera2D(); primaryCamera.has_value())
					{
						Camera2D camera;
						camera.SetViewport(targetWidth, targetHeight);
						camera.SetPosition({ primaryCamera->Transform.Position.X, primaryCamera->Transform.Position.Y });
						camera.SetRotation(primaryCamera->Transform.Rotation);
						camera.SetOrthographicSize(primaryCamera->Camera.OrthographicSize);

						return {
							.Camera = camera,
							.SceneHandle = request.SceneHandle,
							.Target = request.Target,
							.ViewportRect = request.ViewportRect,
							.CameraSource = Ludus::Engine::Graphics::CameraSource::Scene
						};
					}
				}

				// Scene exists but no camera component -> default empty view.
				return {
					.Camera = CreateDefaultCamera(targetWidth, targetHeight),
					.SceneHandle = request.SceneHandle,
					.Target = request.Target,
					.ViewportRect = request.ViewportRect,
					.CameraSource = Ludus::Engine::Graphics::CameraSource::None
				};
			}

			// No camera and no scene -> Overlay and no-scene views.
			return {
				.Camera = CreateDefaultCamera(targetWidth, targetHeight),
				.SceneHandle = std::nullopt,
				.Target = request.Target,
				.ViewportRect = request.ViewportRect,
				.CameraSource = Ludus::Engine::Graphics::CameraSource::None
			};
		}
	};
}
