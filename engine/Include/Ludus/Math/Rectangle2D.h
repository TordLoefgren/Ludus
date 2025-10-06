#pragma once

#include <Ludus/Math/Vector2D.h>

namespace Ludus::Math
{
	class Rectangle2D
	{
	private:
		float m_X;
		float m_Y;
		float m_Width;
		float m_Height;

	public:
		Rectangle2D();
		Rectangle2D(float x, float y, float width, float height);
		Rectangle2D(float x, float y, const Vector2D& size);
		Rectangle2D(const Vector2D& position, float width, float height);
		Rectangle2D(const Vector2D& position, const Vector2D& size);
		~Rectangle2D();

		// Getters.
		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }
		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		// Setters.
		void SetX(float x) { m_X = x; }
		void SetY(float y) { m_Y = y; }
		void SetWidth(float width) { m_Width = width; }
		void SetHeight(float height) { m_Height = height; }

		// Helpers.
		float GetLeft()   const { return m_X; }
		float GetTop()    const { return m_Y; }
		float GetRight()  const { return m_X + m_Width; }
		float GetBottom() const { return m_Y + m_Height; }

		// Methods.
		bool Intersects(const Rectangle2D& other, float epsilon = 0.05f) const;
	};
}
