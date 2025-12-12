#pragma once

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include <Ludus/Engine/Math/Rect.h>
#include <Ludus/Engine/Math/Vector2D.h>

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

		void SetPosition(Ludus::Engine::Math::Vector2D position);
		void SetViewport(int width, int height);
		void SetOrthographicSize(float zoom);
		void SetRotation(float rotation);

		Ludus::Engine::Math::Vector2D GetPosition() const;
		Ludus::Engine::Math::Rect GetWorldRect() const;
		float GetOrthographicSize() const;

		glm::mat4 GetView() const;
		glm::mat4 GetViewProjection() const;

		void RecalculateProjection();
	};
}
