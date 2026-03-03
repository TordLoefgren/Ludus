#pragma once

#include "Types.h"

namespace Ludus::Engine::Scripting::API
{
	struct Registry
	{
	private:
		void (*m_AddFn)(void* context, const ScriptDefinition* definition) = nullptr;
		void* m_Context = nullptr;

	public:
		void Add(const ScriptDefinition* definition)
		{
			if (!m_AddFn || !m_Context || !definition)
			{
				return;
			}

			m_AddFn(m_Context, definition);
		}

		void Bind(void* context, void(*addFn)(void* context, const ScriptDefinition* definition))
		{
			m_Context = context;
			m_AddFn = addFn;
		}
	};
}
