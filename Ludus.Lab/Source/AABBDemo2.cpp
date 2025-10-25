#include <AABBDemo2.h>

namespace Ludus::Lab
{
	AABBDemo2::AABBDemo2(Ludus::Platform::Window& window, int width, int height)
		: m_Window(window), m_Width(width), m_Height(height), m_Shader("Resources/Shaders"), m_Renderer(m_Shader), m_Random(), m_Time(), m_Cooldown(0.05f), m_EntityComponentSystem(), m_CollisionSystem()
	{ }

	void AABBDemo2::Init()
	{
		m_Camera.SetViewport(m_Width, m_Height);

		Ludus::Engine::LayerMask::AddLayer(m_Info.QuadLayerName, m_Info.QuadLayerIndex);
		Ludus::Engine::LayerMask::AddLayer(m_Info.CursorLayerName, m_Info.CursorLayerIndex);

		m_Info.CursorHandle = m_EntityComponentSystem.AddEntity();
		m_EntityComponentSystem.AttachTransform(m_Info.CursorHandle, { m_Width * 0.5f, m_Height * 0.5f }, m_Width * 0.1f);
		m_EntityComponentSystem.AttachCollider(m_Info.CursorHandle, m_Info.CursorLayerIndex, Ludus::Engine::LayerMask::FromIndex(m_Info.QuadLayerIndex));

		m_Renderer.SetClearColor(Ludus::Graphics::Colors::White);

		for (int i = 0; i < 10; i++)
		{
			m_FallingQuads.push_back(CreateQuad());
		}
	}

	void AABBDemo2::Update()
	{
		m_Time.Step();
		m_Cooldown.Step(m_Time);

		// Input Handling.
		if (m_Window.GetInput().GetKeyDown(Ludus::Platform::Key::Escape))
		{
			m_Window.SetWindowShouldClose();
		}

		auto mousePosition = m_Window.GetInput().GetMousePosition();

		if (m_Cooldown.IsElapsed() && m_FallingQuads.size() < m_Info.MaxCount)
		{
			m_FallingQuads.push_back(CreateQuad());
			m_Cooldown.Reset();
		}

		for (auto& quad : m_FallingQuads)
		{
			auto* quadTransformPtr = m_EntityComponentSystem.Transforms.TryGetByOwnerMutable(quad.Handle);
			if (!quadTransformPtr)
			{
				continue;
			}

			auto& quadTransform = *quadTransformPtr;

			if (quadTransform.Position.Y + quadTransform.Scale.Y < 0.0)
			{
				m_EntityComponentSystem.DestroyEntity(quad.Handle);
				auto element = std::remove(m_FallingQuads.begin(), m_FallingQuads.end(), quad);
				m_FallingQuads.erase(element);
			}
		}

		// Movement Integration.
		auto cursorTransform = m_EntityComponentSystem.Transforms.TryGetByOwnerMutable(m_Info.CursorHandle);
		if (!cursorTransform)
		{
			return;
		}

		cursorTransform->Position = { mousePosition.X, m_Height - mousePosition.Y };

		for (auto& quad : m_FallingQuads)
		{
			auto* quadTransformPtr = m_EntityComponentSystem.Transforms.TryGetByOwnerMutable(quad.Handle);
			if (!quadTransformPtr)
			{
				continue;
			}

			auto& quadTransform = *quadTransformPtr;

			auto velocity = Ludus::Math::Vector2D(0.0f, 1.0f) * quad.Speed;

			quadTransform.Position -= velocity * m_Time;
			quad.Color = m_Info.NonCollisionColor;
		}

		// Collision Handling.
		m_CollisionSystem.Step(m_EntityComponentSystem.Colliders, m_EntityComponentSystem.Transforms);
		for (auto& info : m_CollisionSystem.GetCollisionInfo())
		{
			const auto handleA = info.CollisionAOwnerHandle;
			const auto handleB = info.CollisionBOwnerHandle;

			const auto& transformA = m_EntityComponentSystem.Transforms.TryGetByOwner(handleA);
			const auto& transformB = m_EntityComponentSystem.Transforms.TryGetByOwner(handleB);

			if (!transformA || !transformB)
			{
				continue;
			}

			for (auto& quad : m_FallingQuads)
			{
				if (quad.Handle == handleA || quad.Handle == handleB)
				{
					quad.Color = m_Info.CollisionColor;
				}
			}
		}

		// Rendering.
		m_Renderer.BeginScene(m_Camera);
		m_Renderer.Clear();

		m_Renderer.DrawQuad(*cursorTransform, m_Info.CursorColor);

		for (auto& quad : m_FallingQuads)
		{
			auto* quadTransform = m_EntityComponentSystem.Transforms.TryGetByOwner(quad.Handle);
			if (!quadTransform)
			{
				continue;
			}

			m_Renderer.DrawQuad(*quadTransform, quad.Color);
		}

		m_Renderer.EndScene();
	}

	FallingQuad AABBDemo2::CreateQuad()
	{
		auto handle = m_EntityComponentSystem.AddEntity();
		auto scaleX = m_Random.NextFloat((float)m_Width * 0.01f, (float)m_Width * 0.08f);
		auto scaleY = m_Random.NextFloat((float)m_Width * 0.01f, (float)m_Width * 0.08f);
		auto xPosition = m_Random.NextFloat(0.0f, (float)m_Width);
		auto speed = m_Random.NextFloat(50.0f, 250.0f);

		m_EntityComponentSystem.AttachTransform(handle, { xPosition, (float)m_Height + scaleY * 2.0f }, { scaleX, scaleY });
		m_EntityComponentSystem.AttachCollider(handle, m_Info.QuadLayerIndex, Ludus::Engine::LayerMask::FromIndex(m_Info.CursorLayerIndex));

		return FallingQuad(handle, speed, m_Info.NonCollisionColor);
	}
}
