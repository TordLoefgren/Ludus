#include <Ludus/Engine/Graphics/Camera2D.h>

namespace Ludus::Engine::Graphics
{
	Camera2D::Camera2D()
		: m_Position(0.0f, 0.0f), m_ViewportSize(1.0f, 1.0f), m_Projection({ }), m_Rotation(0.0f), m_OrthographicSize(10.0f)
	{ }

	Camera2D::~Camera2D()
	{ }

	void Camera2D::SetPosition(Ludus::Engine::Math::Vector2D position) { m_Position = { position.X, position.Y }; }

	void Camera2D::SetViewport(int width, int height)
	{
		m_ViewportSize = { std::max(1, width), std::max(1, height) };

		RecalculateProjection();
	}

	void Camera2D::SetOrthographicSize(float size)
	{
		m_OrthographicSize = std::max(0.01f, size);

		RecalculateProjection();
	}

	void Camera2D::SetRotation(float rotation) { m_Rotation = glm::radians(rotation); }

	glm::mat4 Camera2D::GetView() const
	{
		// As the othographic size determines the zoom level, we do not need explicit scaling.
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(-m_Position, 0.0f));
		view = glm::rotate(view, -m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));

		return view;
	}

	glm::mat4 Camera2D::GetViewProjection() const
	{
		return m_Projection * GetView();
	}

	Ludus::Engine::Math::Vector2D Camera2D::GetPosition() const { return { m_Position.x, m_Position.y }; }

	Ludus::Engine::Math::Rect Camera2D::GetWorldRect() const
	{
		const auto aspect = m_ViewportSize.x / m_ViewportSize.y;
		const auto halfHeight = m_OrthographicSize;
		const auto halfWidth = halfHeight * aspect;

		const auto width = halfWidth * 2.0f;
		const auto height = halfHeight * 2.0f;

		return { { m_Position.x, m_Position.y }, { width, height } };
	}

	float Camera2D::GetOrthographicSize() const { return m_OrthographicSize; }

	void Camera2D::RecalculateProjection()
	{
		// Using the Unity default half-size: 
		// https://docs.unity3d.com/ScriptReference/Camera-orthographicSize.html.

		const auto aspect = m_ViewportSize.x / m_ViewportSize.y;
		const auto halfHeight = m_OrthographicSize;
		const auto halfWidth = halfHeight * aspect;

		m_Projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
	}
}
