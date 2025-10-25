#pragma once

#include <Ludus/Core/SystemContext.h>

namespace Ludus::Core
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual void OnAttach(SystemContext& context) { m_SystemContext = &context; }
		virtual void FixedUpdate(float fixedTime) { };
		virtual void Update(float deltaTime) { };

	protected:
		Ludus::Core::SystemContext* m_SystemContext = nullptr;
	};
}
