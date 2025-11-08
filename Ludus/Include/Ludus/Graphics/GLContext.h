#pragma once

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Events/Event.h>
#include <Ludus/Events/Eventhandler.h>

namespace Ludus::Graphics
{
	class GLContext : Ludus::Events::Eventhandler
	{
	public:
		static void Init();
		static void EnableBlending();
		static void SetBlendAlpha();

		virtual bool ProcessEvent(const Ludus::Events::Event& event) override;
	};
}
