#include "Ludus/Lab/Core/Scene.h"

namespace Ludus::Lab::Core
{
	Scene::Scene()
		: m_Random(), m_Cooldown(0.05f)
	{ }

	void Scene::OnAttachImpl()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;
		auto handle = ecs.AddEntity();

		const auto& windowOptions = m_SystemContext->Window.GetOptions();
		auto [currentWidth, currentHeight] = m_SystemContext->Window.GetFramebufferSize();

		Ludus::Engine::LayerMask::AddLayer(m_Info.QuadLayerName, m_Info.QuadLayerIndex);
		Ludus::Engine::LayerMask::AddLayer(m_Info.CursorLayerName, m_Info.CursorLayerIndex);

		m_Info.CursorHandle = ecs.AddEntity();
		ecs.AttachCollider(
			m_Info.CursorHandle,
			m_Info.CursorLayerIndex,
			Ludus::Engine::LayerMask::FromIndex(m_Info.QuadLayerIndex),
			true
		);
		ecs.AttachRigidBody(m_Info.CursorHandle, { 0.0f });
		ecs.AttachSprite(m_Info.CursorHandle, Ludus::Graphics::Shape::Rect, m_Info.CursorColor);
		ecs.AttachTransform(m_Info.CursorHandle, { currentWidth * 0.5f, currentHeight * 0.5f }, currentWidth * 0.1f);

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

		// Input Handling.
		if (input.GetKeyDown(Ludus::Platform::Key::Escape))
		{
			m_SystemContext->Window.SetWindowShouldClose();
		}

		if (input.GetMouseButtonDown(Ludus::Platform::MouseButton::Left))
		{
			LUDUS_LOG_INFO("Mouse clicked in Scene.");
		}

		auto mousePosition = input.GetMousePosition();

		if (m_Cooldown.IsElapsed() && m_FallingQuads.size() < m_Info.MaxCount)
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

		// Movement Integration.
		auto [currentWidth, currentHeight] = m_SystemContext->Window.GetFramebufferSize();

		auto cursorTransform = ecs.Transforms.TryGetByOwnerMutable(m_Info.CursorHandle);
		if (!cursorTransform)
		{
			return;
		}

		cursorTransform->Position = { mousePosition.X, currentHeight - mousePosition.Y };

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
	}

	FallingQuad Scene::CreateQuad()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;
		auto& options = m_SystemContext->Window.GetOptions();

		auto scaleX = m_Random.NextFloat((float)options.Width * 0.01f, (float)options.Width * 0.08f);
		auto scaleY = m_Random.NextFloat((float)options.Width * 0.01f, (float)options.Width * 0.08f);
		auto xPosition = m_Random.NextFloat(0.0f, (float)options.Width);
		auto speed = m_Random.NextFloat(10.0f, 25.0f);

		auto handle = ecs.AddEntity();
		ecs.AttachCollider(handle, m_Info.QuadLayerIndex, Ludus::Engine::LayerMask::FromIndex(m_Info.CursorLayerIndex));
		ecs.AttachRigidBody(handle, { 0.0f }, Ludus::Physics::Core::BodyType::Dynamic, speed);
		ecs.AttachSprite(handle, Ludus::Graphics::Shape::Rect, m_Info.NonCollisionColor);
		ecs.AttachTransform(handle, { xPosition, (float)options.Height + scaleY * 2.0f }, { scaleX, scaleY });

		return FallingQuad { handle, speed };
	}
}
