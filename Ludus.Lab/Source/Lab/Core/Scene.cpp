#include "Lab/Core/Scene.h"

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

		Ludus::Engine::LayerMask::AddLayer(m_Info.QuadLayerName, m_Info.QuadLayerIndex);
		Ludus::Engine::LayerMask::AddLayer(m_Info.CursorLayerName, m_Info.CursorLayerIndex);

		m_Info.CursorHandle = ecs.AddEntity();
		ecs.AttachCollider(
			m_Info.CursorHandle,
			m_Info.CursorLayerIndex,
			Ludus::Engine::LayerMask::FromIndex(m_Info.QuadLayerIndex),
			Ludus::Physics::Core::BodyType::Kinematic,
			true
		);
		ecs.AttachSprite(m_Info.CursorHandle, Ludus::Graphics::Shape::Rect, m_Info.CursorColor);
		ecs.AttachTransform(m_Info.CursorHandle, { windowOptions.Width * 0.5f, windowOptions.Height * 0.5f }, windowOptions.Width * 0.1f);

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
		auto cursorTransform = ecs.Transforms.TryGetByOwnerMutable(m_Info.CursorHandle);
		if (!cursorTransform)
		{
			return;
		}

		cursorTransform->Position = { mousePosition.X, m_SystemContext->Window.GetOptions().Height - mousePosition.Y };

		for (auto& quad : m_FallingQuads)
		{
			auto* quadTransformPtr = ecs.Transforms.TryGetByOwnerMutable(quad.Handle);
			if (!quadTransformPtr)
			{
				continue;
			}

			auto& quadTransform = *quadTransformPtr;

			auto* quadSpritePtr = ecs.Sprites.TryGetByOwnerMutable(quad.Handle);
			if (!quadSpritePtr)
			{
				continue;
			}

			auto& quadSprite = *quadSpritePtr;

			auto velocity = Ludus::Math::Vector2D(0.0f, 1.0f) * quad.Speed;
			quadTransform.Position -= velocity * deltaTime;

			quadSprite.Color = m_SystemContext->PhysicsQueries->IsTriggering(quad.Handle, m_Info.CursorHandle)
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
		auto speed = m_Random.NextFloat(50.0f, 250.0f);

		auto handle = ecs.AddEntity();
		ecs.AttachCollider(handle, m_Info.QuadLayerIndex, Ludus::Engine::LayerMask::FromIndex(m_Info.CursorLayerIndex));
		ecs.AttachSprite(handle, Ludus::Graphics::Shape::Rect, m_Info.NonCollisionColor);
		ecs.AttachTransform(handle, { xPosition, (float)options.Height + scaleY * 2.0f }, { scaleX, scaleY });

		return FallingQuad { handle, speed };
	}
}
