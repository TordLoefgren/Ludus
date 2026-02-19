#pragma once

namespace Ludus::UI::Scope
{
	class DisabledScope
	{
	private:
		bool m_Active = false;

	public:
		explicit DisabledScope(bool disabled);

		~DisabledScope();

		explicit operator bool() const { return m_Active; }
	};
}
