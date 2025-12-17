#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Scope
{
	class ListBoxScope
	{
	private:
		bool m_Open = false;

	public:
		explicit ListBoxScope(const char* label, Ludus::Engine::Math::Vector2D size = { 0.0f, 0.0f });

		~ListBoxScope();

		explicit operator bool() const { return m_Open; }
	};
}
