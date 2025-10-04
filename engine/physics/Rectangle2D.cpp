#include "Rectangle2D.h"

Rectangle2D::Rectangle2D()
	: m_X(0.0f), m_Y(0.0f), m_Width(0.0f), m_Height(0.0f)
{ }

Rectangle2D::Rectangle2D(float x, float y, float width, float height)
	: m_X(x), m_Y(y), m_Width(width), m_Height(height)
{ }

Rectangle2D::Rectangle2D(float x, float y, const Vector2D& size)
	: m_X(x), m_Y(y), m_Width(size.X), m_Height(size.Y)
{ }

Rectangle2D::Rectangle2D(const Vector2D& position, float width, float height)
	: m_X(position.X), m_Y(position.Y), m_Width(width), m_Height(height)
{ }

Rectangle2D::Rectangle2D(const Vector2D& position, const Vector2D& size)
	: m_X(position.X), m_Y(position.Y), m_Width(size.X), m_Height(size.Y)
{ }

Rectangle2D::~Rectangle2D() = default;

bool Rectangle2D::Intersects(const Rectangle2D& other, float epsilon) const
{
	return GetX() < other.GetX() + other.GetWidth() - epsilon &&
		GetX() + GetWidth() > other.GetX() + epsilon &&
		GetY() < other.GetY() + other.GetHeight() - epsilon &&
		GetY() + GetHeight() > other.GetY() + epsilon;
}
