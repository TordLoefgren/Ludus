#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::UI::Scope
{
	class MenuBarScope
	{
	private:
		bool m_Open = false;

	public:
		explicit MenuBarScope();

		~MenuBarScope();

		explicit operator bool() const { return m_Open; }
	};
}
