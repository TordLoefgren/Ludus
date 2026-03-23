#pragma once

namespace Ludus::Engine::Runtime
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		void OnAttach() { OnAttachImpl(); }
		void OnDetach() { OnDetachImpl(); }

		void OnTransitionEnter() { OnTransitionEnterImpl(); }
		void OnTransitionExit() { OnTransitionExitImpl(); }

		void FixedUpdate(float fixedDeltaTime) { FixedUpdateImpl(fixedDeltaTime); }
		void Update(float deltaTime) { UpdateImpl(deltaTime); }
		void Render() { RenderImpl(); }

		void BeginFrame() { BeginFrameImpl(); };
		void EndFrame() { EndFrameImpl(); };

	protected:
		virtual void OnAttachImpl() { };
		virtual void OnDetachImpl() { };

		virtual void OnTransitionEnterImpl() { };
		virtual void OnTransitionExitImpl() { };

		virtual void FixedUpdateImpl(float fixedDeltaTime) { };
		virtual void UpdateImpl(float deltaTime) { };
		virtual void RenderImpl() { };

		virtual void BeginFrameImpl() { };
		virtual void EndFrameImpl() { };
	};
}
