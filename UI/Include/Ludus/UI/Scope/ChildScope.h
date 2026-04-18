#pragma once

#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Scope
{
	constexpr Ludus::UI::Flags::Child DefaultChildFlags = Ludus::UI::Flags::Child::None;
	constexpr Ludus::UI::Flags::Window DefaultChildWindowFlags = Ludus::UI::Flags::Window::None;

	class ChildScope
	{
	private:
		bool m_Active = false;

	public:
		explicit ChildScope(
			const char* id,
			const Ludus::Engine::Math::Vector2D size,
			Ludus::UI::Flags::Child childFlags = DefaultChildFlags,
			Ludus::UI::Flags::Window windowFlags = DefaultChildWindowFlags
		);

		~ChildScope();

		explicit operator bool() const { return m_Active; }
	};
}
