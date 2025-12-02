#include <Ludus/Graphics/Camera2D.h>

namespace Ludus::Graphics
{
	Camera2D::Camera2D()
		: m_Position(0.0f, 0.0f), m_Zoom(1.0f), m_Rotation(0.0f), m_Projection(1.0f), m_ScreenCenter(0.0f, 0.0f)
	{ }

	Camera2D::~Camera2D()
	{ }

	void Camera2D::SetPosition(glm::vec2 position) { m_Position = position; }

	void Camera2D::SetRotation(float rotation) { m_Rotation = glm::radians(rotation); }

	void Camera2D::SetZoom(float zoom)
	{
		m_Zoom = std::max(0.01f, zoom);
	}

	void Camera2D::SetViewport(int width, int height)
	{
		m_ScreenCenter = { 0.5f * width, 0.5f * height };
		m_Projection = glm::ortho(0.0f, float(width), 0.0f, float(height), -1.0f, 1.0f);
	}

	glm::mat4 Camera2D::GetView() const
	{
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(m_ScreenCenter, 0.0f));
		view = glm::rotate(view, -m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		view = glm::scale(view, glm::vec3(m_Zoom, m_Zoom, 1.0f));
		view = glm::translate(view, glm::vec3(-m_ScreenCenter, 0.0f));
		view = glm::translate(view, glm::vec3(-m_Position, 0.0f));

		return view;
	}

	glm::mat4 Camera2D::GetViewProjection() const
	{
		return m_Projection * GetView();
	}
}
