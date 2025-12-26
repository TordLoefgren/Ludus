#include "pch.h"

#include <optional>

#include <Ludus/Lab/Core/Scene.h>

namespace Ludus::Lab::Core
{
	Scene::Scene()
		: m_Random(), m_Cooldown(0.05f)
	{ }

	void Scene::UpdateWorld()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		auto* cameraComponentPtr = ecs.Cameras.TryGetByOwnerMutable(m_Info.CameraHandle);
		auto* cameraTransformPtr = ecs.Transforms.TryGetByOwnerMutable(m_Info.CameraHandle);

		if (!cameraComponentPtr || !cameraTransformPtr)
		{
			return;
		}

		const auto [framebufferWidth, framebufferHeight] = m_SystemContext->Window.GetFramebufferSize();
		cameraComponentPtr->SetWorldFromViewport(static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight));

		m_World.HalfWidth = cameraComponentPtr->HalfWorldWidth();
		m_World.HalfHeight = cameraComponentPtr->HalfWorldHeight();
		m_World.Width = m_World.HalfWidth * 2.0f;
		m_World.Height = m_World.HalfHeight * 2.0f;

		cameraTransformPtr->Position = { m_World.HalfWidth, m_World.HalfHeight };
	}

	void Scene::OnAttachImpl()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		// Setup camera.
		m_World.OrthographicSize = 8.0f;

		m_Info.CameraHandle = ecs.AddEntity();
		ecs.AttachCamera(m_Info.CameraHandle, m_World.OrthographicSize);
		ecs.AttachTransform(m_Info.CameraHandle, { m_World.HalfWidth, m_World.HalfHeight });

		UpdateWorld();

		Ludus::Engine::Physics::Core::LayerMask::AddLayer(m_Info.QuadLayerName, m_Info.QuadLayerIndex);
		Ludus::Engine::Physics::Core::LayerMask::AddLayer(m_Info.CursorLayerName, m_Info.CursorLayerIndex);

		m_Info.CursorHandle = ecs.AddEntity();
		ecs.AttachCollider(
			m_Info.CursorHandle,
			m_Info.CursorLayerIndex,
			Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_Info.QuadLayerIndex),
			true
		);
		ecs.AttachRigidBody(m_Info.CursorHandle, { 0.0f });

		ecs.AttachSprite(m_Info.CursorHandle, Ludus::Engine::Graphics::Shape::Quad, m_Info.CursorColor);
		ecs.AttachTransform(m_Info.CursorHandle, { 0.0f }, m_World.Height * 0.1f);

		if (m_World.Width <= 0.0f || m_World.Height <= 0.0f)
		{
			return;
		}

		for (int i = 0; i < 10; i++)
		{
			m_FallingQuads.push_back(CreateQuad());
		}
	}

	void Scene::UpdateImpl(float deltaTime)
	{
		auto& input = m_SystemContext->Input;
		auto& ecs = m_SystemContext->EntityComponentSystem;
		m_Cooldown.Step(deltaTime);

		UpdateWorld();

		// Input Handling.
		if (input.GetKeyDown(Ludus::Engine::Platform::Key::Escape))
		{
			m_SystemContext->Window.SetWindowShouldClose();
		}

		// Movement Integration.
		auto cursorTransform = ecs.Transforms.TryGetByOwnerMutable(m_Info.CursorHandle);
		if (!cursorTransform)
		{
			return;
		}

		auto mousePosition = input.GetMousePosition();

		auto [windowWidth, windowHeight] = m_SystemContext->Window.GetWindowSize();
		auto xNormalized = mousePosition.X / windowWidth;
		auto yNormalized = mousePosition.Y / windowHeight;

		auto x = xNormalized * m_World.Width;
		auto y = yNormalized * m_World.Height;

		cursorTransform->Position = { x, m_World.Height - y };

		for (auto& quad : m_FallingQuads)
		{
			auto* quadSpritePtr = ecs.Sprites.TryGetByOwnerMutable(quad.Handle);
			if (!quadSpritePtr)
			{
				continue;
			}

			quadSpritePtr->Color = m_SystemContext->PhysicsQueries->IsTriggering(quad.Handle, m_Info.CursorHandle)
				? m_Info.CollisionColor
				: m_Info.NonCollisionColor;
		}

		if (m_Cooldown.IsElapsed() && m_FallingQuads.size() < m_Info.MaxCount && m_World.Width > 0.0f && m_World.Height > 0.0f)
		{
			m_FallingQuads.push_back(CreateQuad());
			m_Cooldown.Reset();
		}

		for (auto& quad : m_FallingQuads)
		{
			auto* quadTransformPtr = ecs.Transforms.TryGetByOwnerMutable(quad.Handle);
			if (!quadTransformPtr)
			{
				continue;
			}

			auto& quadTransform = *quadTransformPtr;

			if (quadTransform.Position.Y + quadTransform.Scale.Y < 0.0)
			{
				ecs.DestroyEntity(quad.Handle);
				auto element = std::remove(m_FallingQuads.begin(), m_FallingQuads.end(), quad);
				m_FallingQuads.erase(element);
			}
		}

		const auto* cameraComponentPtr = ecs.Cameras.TryGetByOwner(m_Info.CameraHandle);
		const auto* cameraTransformPtr = ecs.Transforms.TryGetByOwner(m_Info.CameraHandle);
		if (!cameraComponentPtr || !cameraTransformPtr)
		{
			return;
		}

		const auto [framebufferWidth, framebufferHeight] = m_SystemContext->Window.GetFramebufferSize();

		Ludus::Engine::Graphics::Camera2D camera;
		camera.SetViewport(framebufferWidth, framebufferHeight);
		camera.SetPosition({ cameraTransformPtr->Position.X, cameraTransformPtr->Position.Y });
		camera.SetRotation(cameraTransformPtr->Rotation);
		camera.SetOrthographicSize(cameraComponentPtr->OrthographicSize);

		m_SystemContext->RenderViews.RegisterFullscreen(std::nullopt, camera, m_SystemContext->WindowRenderTarget);
	}

	FallingQuad Scene::CreateQuad()
	{
		auto scaleX = m_Random.NextFloat(m_World.Width * 0.01f, m_World.Width * 0.08f);
		auto scaleY = m_Random.NextFloat(m_World.Height * 0.01f, m_World.Height * 0.08f);
		auto xPosition = m_Random.NextFloat(0.0f, m_World.Width);
		auto gravityScale = m_Random.NextFloat(0.1f, 2.0f);

		auto& ecs = m_SystemContext->EntityComponentSystem;

		auto handle = ecs.AddEntity();
		ecs.AttachCollider(handle, m_Info.QuadLayerIndex, Ludus::Engine::Physics::Core::LayerMask::FromIndex(m_Info.CursorLayerIndex));
		ecs.AttachRigidBody(handle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Dynamic, gravityScale);
		ecs.AttachSprite(handle, Ludus::Engine::Graphics::Shape::Quad, m_Info.NonCollisionColor);
		ecs.AttachTransform(handle, { xPosition, m_World.Height + scaleY }, { scaleX, scaleY });

		return FallingQuad { handle };
	}
}
