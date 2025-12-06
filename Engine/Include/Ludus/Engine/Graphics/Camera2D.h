#pragma once

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace Ludus::Engine::Graphics
{
	class Camera2D
	{
	private:
		glm::vec2 m_Position;
		glm::vec2 m_ViewportSize;
		glm::mat4 m_Projection;
		float m_Rotation;
		float m_OrthographicSize;

	public:
		Camera2D();
		~Camera2D();

		void SetPosition(glm::vec2 position);
		void SetViewport(int width, int height);
		void SetOrthographicSize(float zoom);
		void SetRotation(float rotation);

		glm::mat4 GetView() const;
		glm::mat4 GetViewProjection() const;
		float GetOrthographicSize() const;

		void RecalculateProjection();
	};
}
