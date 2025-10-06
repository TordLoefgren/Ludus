#pragma once

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace Ludus::Graphics
{
	class Camera2D
	{
	private:
		glm::vec2 m_Position;
		glm::mat4 m_Projection;
		glm::vec2 m_ScreenCenter;
		float m_Zoom;
		float m_Rotation;

	public:
		Camera2D();
		~Camera2D();

		void SetPosition(glm::vec2 position);
		void SetRotation(float rotation);
		void SetZoom(float zoom);
		void SetViewport(int width, int height);

		glm::mat4 GetView() const;
		glm::mat4 GetViewProjection() const;
	};
}
