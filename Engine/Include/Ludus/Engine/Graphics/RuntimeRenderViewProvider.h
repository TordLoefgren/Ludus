#pragma once

#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Graphics/Camera2DComponent.h>
#include <Ludus/Engine/Graphics/IRenderViewProvider.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Math/Transform2D.h>

namespace Ludus::Engine::Graphics
{
	struct CameraComponents
	{
		const Camera2DComponent* CameraComponent;
		const Ludus::Engine::Math::Transform2D* TransformComponent;
	};

	struct RuntimeRenderViewProvider : public IRenderViewProvider
	{
	private:
		std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> m_Target;

		CameraComponents GetMainCamera(const Ludus::Engine::Core::EntityComponentSystem& ecs)
		{
			for (auto& camera : ecs.Cameras.View())
			{
				if (camera.IsMainCamera)
				{
					const auto* transformPtr = ecs.Transforms.TryGetByOwner(camera.OwnerHandle);
					if (!transformPtr)
					{
						continue;
					}

					return { &camera, transformPtr };
				}
			}

			return { };
		}

	public:
		RuntimeRenderViewProvider(std::shared_ptr<RenderTarget> target)
			: m_Target(std::move(target))
		{ }

		virtual bool BuildRenderView(const Ludus::Engine::Core::SystemContext& context, RenderView2D& outRenderView) override
		{
			auto& ecs = context.EntityComponentSystem;

			const auto [cameraComponent, transformComponent] = GetMainCamera(ecs);
			if (!cameraComponent || !transformComponent)
			{
				LUDUS_LOG_WARN("No primary camera available.");
				return false;
			}

			auto [width, height] = context.Window.GetFramebufferSize();

			Camera2D camera;
			camera.SetViewport((int)width, (int)height);
			camera.SetPosition({ transformComponent->Position.X, transformComponent->Position.Y });
			camera.SetRotation(transformComponent->Rotation);
			camera.SetOrthographicSize(cameraComponent->OrthographicSize);

			outRenderView.Camera = camera;
			outRenderView.Target = m_Target;
			outRenderView.ViewportPosition = { 0.0f, 0.0f };
			outRenderView.ViewportSize = { (float)width, (float)height };

			return true;
		}
	};
}
