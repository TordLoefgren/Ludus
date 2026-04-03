#pragma once

#include <optional>
#include <utility>

#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/RenderViewConfiguration.h>
#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/ISystem.h>

namespace Ludus::Engine::Graphics
{
	class RenderViewSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		Ludus::Engine::Graphics::RenderViewConfiguration m_RenderViewConfiguration;
		Ludus::Engine::Core::RenderViewRegistry& m_RenderViewRegistry;
		Ludus::Engine::Core::RenderViewRequestRegistry& m_RenderViewRequestRegistry;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;

	public:
		RenderViewSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration,
			Ludus::Engine::Core::RenderViewRegistry& renderViewRegistry,
			Ludus::Engine::Core::RenderViewRequestRegistry& renderViewRequestRegistry,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry
		) :
			m_HostContext(hostContext),
			m_RenderViewConfiguration(std::move(renderViewConfiguration)),
			m_RenderViewRegistry(renderViewRegistry),
			m_RenderViewRequestRegistry(renderViewRequestRegistry),
			m_SceneRegistry(sceneRegistry)
		{ }

		~RenderViewSystem() = default;

	protected:
		virtual void BeginFrameImpl() override
		{
			m_RenderViewRegistry.Clear();
			m_RenderViewRequestRegistry.Clear();
		}

		virtual void RenderImpl() override
		{
			// Pull render view requests.
			auto renderRequests = m_RenderViewRequestRegistry.View();
			if (renderRequests.empty())
			{
				// Check configuration.
				if (m_RenderViewConfiguration.EnableDefaultViewFallback)
				{
					const auto [framebufferWidth, framebufferHeight] = m_HostContext.GetFramebufferSize();
					auto camera = CreateDefaultCamera(framebufferWidth, framebufferHeight);

					auto& windowRenderTarget = m_HostContext.GetMainRenderTarget();
					m_RenderViewRegistry.RegisterFullscreen(std::nullopt, camera, windowRenderTarget);
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
				auto resolvedRequest = ResolveRequest(m_SceneRegistry, request);
				m_RenderViewRegistry.Register(std::move(resolvedRequest));
			}
		}

		static Ludus::Engine::Graphics::Camera2D CreateDefaultCamera(int targetWidth, int targetHeight)
		{
			Camera2D camera;
			camera.SetViewport(targetWidth, targetHeight);
			camera.SetPosition({ 0, 0 });
			camera.SetRotation(0);
			camera.SetOrthographicSize(10);

			return camera;
		}

		static RenderView2D ResolveRequest(Ludus::Engine::Core::SceneRegistry& sceneRegistry, const RenderViewRequest2D& request)
		{
			auto [targetWidth, targetHeight] = request.Target->Framebuffer.GetSize();

			// Explicit camera -> Tool camera.
			if (request.Camera)
			{
				auto camera = *request.Camera;
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
			if (request.SceneHandle)
			{
				auto* scene = sceneRegistry.TryGetScene(*request.SceneHandle);
				if (scene)
				{
					if (auto primaryCamera = scene->TryGetPrimaryCamera2D(); primaryCamera)
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
