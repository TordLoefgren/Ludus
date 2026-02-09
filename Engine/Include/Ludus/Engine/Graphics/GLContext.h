#pragma once

#include <Ludus/Engine/Events/Event.h>
#include <Ludus/Engine/Events/Eventhandler.h>

namespace Ludus::Engine::Graphics
{
	class GLContext : Ludus::Engine::Events::Eventhandler
	{
	public:
		void Init();
		void EnableBlending();
		void SetBlendAlpha();
		void SetViewport(int width, int height);

		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;
	};
}
