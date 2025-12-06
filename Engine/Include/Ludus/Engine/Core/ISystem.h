#pragma once

#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Core
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		void OnAttach(SystemContext& context)
		{
			m_SystemContext = &context;
			OnAttachImpl();
		}

		void OnDetach()
		{
			OnDetachImpl();
			m_SystemContext = nullptr;
		}

		void OnTransitionEnter() { OnTransitionEnterImpl(); }
		void OnTransitionExit() { OnTransitionExitImpl(); }

		void FixedUpdate(float fixedTime) { FixedUpdateImpl(fixedTime); }
		void Update(float deltaTime) { UpdateImpl(deltaTime); }
		void Render() { RenderImpl(); }

	protected:
		virtual void OnAttachImpl() { };
		virtual void OnDetachImpl() { };

		virtual void OnTransitionEnterImpl() { };
		virtual void OnTransitionExitImpl() { };

		virtual void FixedUpdateImpl(float fixedTime) { };
		virtual void UpdateImpl(float deltaTime) { };
		virtual void RenderImpl() { };

		Ludus::Engine::Core::SystemContext* m_SystemContext = nullptr;
	};
}
