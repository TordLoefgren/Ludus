#pragma once

#include <Ludus/Engine/Events/EventHandler.h>

namespace Ludus::Engine::Events
{
	struct Event;
}

namespace Ludus::Engine::Graphics
{
	class GLContext : public Ludus::Engine::Events::EventHandler
	{
	public:
		void Init();
		void EnableBlending();
		void SetBlendAlpha();
		void SetViewport(int width, int height);

		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;
	};
}
