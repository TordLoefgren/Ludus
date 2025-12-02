#pragma once

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Events/Event.h>
#include <Ludus/Events/Eventhandler.h>

namespace Ludus::Graphics
{
	class GLContext : Ludus::Events::Eventhandler
	{
	public:
		void Init();
		void EnableBlending();
		void SetBlendAlpha();
		void SetViewport(int width, int height);

		virtual bool ProcessEvent(const Ludus::Events::Event& event) override;
	};
}
