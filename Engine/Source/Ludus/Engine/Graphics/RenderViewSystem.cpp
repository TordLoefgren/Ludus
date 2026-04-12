#include "pch.h"

#include <optional>
#include <utility>

#include <Ludus/Engine/Core/RenderViewRegistry.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/CameraSource.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>
#include <Ludus/Engine/Graphics/RenderViewSystem.h>
#include <Ludus/Engine/Runtime/IHostContext.h>

namespace Ludus::Engine::Graphics
{
	namespace
	{
		Camera2D CreateDefaultCamera(int targetWidth, int targetHeight)
		{
			Camera2D camera;
			camera.SetViewport(targetWidth, targetHeight);
			camera.SetPosition({ 0, 0 });
			camera.SetRotation(0);
			camera.SetOrthographicSize(10);

			return camera;
		}

		RenderView2D ResolveRequest(
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			const RenderViewRequest2D& request
		)
		{
			auto [targetWidth, targetHeight] = request.Target->Framebuffer.GetSize();

			if (request.Camera)
			{
				auto camera = *request.Camera;
				camera.SetViewport(targetWidth, targetHeight);

				return {
					.Camera = camera,
					.SceneId = request.SceneId,
					.Target = request.Target,
					.ViewportRect = request.ViewportRect,
					.CameraSource = Ludus::Engine::Graphics::CameraSource::Explicit
				};
			}

			if (request.SceneId)
			{
				auto* scene = sceneRegistry.TryGetScene(*request.SceneId);
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
							.SceneId = request.SceneId,
							.Target = request.Target,
							.ViewportRect = request.ViewportRect,
							.CameraSource = Ludus::Engine::Graphics::CameraSource::Scene
						};
					}
				}

				return {
					.Camera = CreateDefaultCamera(targetWidth, targetHeight),
					.SceneId = request.SceneId,
					.Target = request.Target,
					.ViewportRect = request.ViewportRect,
					.CameraSource = Ludus::Engine::Graphics::CameraSource::None
				};
			}

			return {
				.Camera = CreateDefaultCamera(targetWidth, targetHeight),
				.SceneId = std::nullopt,
				.Target = request.Target,
				.ViewportRect = request.ViewportRect,
				.CameraSource = Ludus::Engine::Graphics::CameraSource::None
			};
		}
	}

	RenderViewSystem::RenderViewSystem(
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

	void RenderViewSystem::BeginFrameImpl()
	{
		m_RenderViewRegistry.Clear();
		m_RenderViewRequestRegistry.Clear();
	}

	void RenderViewSystem::RenderImpl()
	{
		auto renderRequests = m_RenderViewRequestRegistry.View();
		if (renderRequests.empty())
		{
			if (m_RenderViewConfiguration.EnableDefaultViewFallback)
			{
				const auto [framebufferWidth, framebufferHeight] = m_HostContext.GetFramebufferSize();
				auto camera = CreateDefaultCamera(framebufferWidth, framebufferHeight);

				auto& windowRenderTarget = m_HostContext.GetMainRenderTarget();
				m_RenderViewRegistry.RegisterFullscreen(std::nullopt, camera, windowRenderTarget);
			}

			return;
		}

		for (const auto& request : renderRequests)
		{
			if (!request.Target)
			{
				LUDUS_LOG_WARN("No target available for render view request.");
				continue;
			}

			auto resolvedRequest = ResolveRequest(m_SceneRegistry, request);
			m_RenderViewRegistry.Register(std::move(resolvedRequest));
		}
	}
}
