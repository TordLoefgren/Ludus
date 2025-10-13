#include <AABBDemo1.h>

namespace Ludus::Lab
{
	void AABBDemo1::Init()
	{
		m_Camera.SetViewport(m_Width, m_Height);

		m_Info.CursorHandle = m_Scene.AddGameObject();
		m_Info.BoxHandle = m_Scene.AddGameObject();

		m_Scene.AttachTransform(m_Info.CursorHandle, { m_Width * 0.5f, m_Height * 0.5f }, { m_Width * 0.1f, m_Width * 0.1f });
		m_Scene.AttachTransform(m_Info.BoxHandle, { m_Width * 0.5f, m_Height * 0.5f }, { m_Width * 0.25f, m_Height * 0.25f });

		Ludus::Engine::LayerMask::AddLayer(m_Info.BoxLayerName, m_Info.BoxLayerIndex);
		Ludus::Engine::LayerMask::AddLayer(m_Info.CursorLayerName, m_Info.CursorLayerIndex);

		m_Scene.AttachCollider(m_Info.CursorHandle, m_Info.CursorLayerIndex, Ludus::Engine::LayerMask::FromIndex(m_Info.BoxLayerIndex));
		m_Scene.AttachCollider(m_Info.BoxHandle, m_Info.BoxLayerIndex, Ludus::Engine::LayerMask::FromIndex(m_Info.CursorLayerIndex));

		m_Renderer.SetClearColor(Ludus::Graphics::Colors::Gray);
		m_Renderer.SetLineWidth(2.0f);
		m_Renderer.SetStrokeThickness(2.0f);
	}

	void AABBDemo1::Update()
	{
		// Input Handling.
		if (m_Window.GetInput().GetKeyDown(Ludus::Platform::Key::Escape))
		{
			m_Window.SetWindowShouldClose();
		}

		if (m_Window.GetInput().GetMouseButtonDown(Ludus::Platform::MouseButton::Left))
		{
			m_Info.IsCircle = !m_Info.IsCircle;
		}

		auto mousePosition = m_Window.GetInput().GetMousePosition();

		// Movement Integration.
		auto* cursorTransform = m_Scene.Transforms.TryGetByOwnerMutable(m_Info.CursorHandle);
		auto* boxTransform = m_Scene.Transforms.TryGetByOwnerMutable(m_Info.BoxHandle);
		if (!boxTransform || !cursorTransform)
		{
			return;
		}

		cursorTransform->Position.X = mousePosition.X;
		cursorTransform->Position.Y = m_Height - mousePosition.Y;

		// Collision Handling.
		m_CollisionSystem.Step(m_Scene.Colliders, m_Scene.Transforms, true);
		m_Info.IsColliding = m_CollisionSystem.GetCollisionInfo().size() > 0;

		for (auto& info : m_CollisionSystem.GetOverlapInfo())
		{
			auto& overlapInfo = m_Info.OverlapInfo;
			overlapInfo.IsAAboveB = info.IsAAboveB;
			overlapInfo.IsABelowB = info.IsABelowB;
			overlapInfo.IsALeftOfB = info.IsALeftOfB;
			overlapInfo.IsARightOfB = info.IsARightOfB;
			overlapInfo.IsXAxisCollision = info.IsXAxisCollision;
			overlapInfo.IsYAxisCollision = info.IsYAxisCollision;
		}

		// Rendering.
		m_Renderer.BeginScene(m_Camera);
		m_Renderer.Clear();

		auto color = m_Info.IsColliding ? m_Info.CollisionColor : m_Info.NonCollisionColor;

		m_Renderer.DrawQuad(*boxTransform, color, nullptr, false);
		DrawCursors(m_Renderer, *boxTransform, color);

		if (m_Info.IsCircle)
		{
			m_Renderer.DrawCircle(*cursorTransform, color, false);
		}
		else
		{
			m_Renderer.DrawQuad(*cursorTransform, color, nullptr, false);
		}
		DrawCursors(m_Renderer, *cursorTransform, color);
		m_Renderer.EndScene();
	}

	void AABBDemo1::DrawCursor(Ludus::Graphics::Renderer2D& renderer, float x, float y, Ludus::Graphics::Color color)
	{
		renderer.DrawLine(x, 0.0f, x, (float)m_Height, color);
		renderer.DrawLine(0.0f, y, (float)m_Width, y, color);
	}

	void AABBDemo1::DrawCursors(Ludus::Graphics::Renderer2D& renderer, const Ludus::Math::Transform2D& transform, Ludus::Graphics::Color color)
	{
		auto halfExtent = transform.Scale * 0.5f;

		DrawCursor(renderer, transform.Position.X - halfExtent.X, transform.Position.Y - halfExtent.Y, color);
		DrawCursor(renderer, transform.Position.X + halfExtent.X, transform.Position.Y - halfExtent.Y, color);
		DrawCursor(renderer, transform.Position.X - halfExtent.X, transform.Position.Y + halfExtent.Y, color);
		DrawCursor(renderer, transform.Position.X + halfExtent.X, transform.Position.Y + halfExtent.Y, color);
	}

	AABBDemo1::AABBDemo1(Ludus::Platform::Window& window, int width, int height)
		: m_Window(window), m_Width(width), m_Height(height), m_Shader("Resources/Shaders"), m_Renderer(m_Shader), m_Scene(), m_CollisionSystem()
	{ }
}
