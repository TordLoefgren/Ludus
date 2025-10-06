#pragma once

#include <Ludus/Engine/Utilities.h>

namespace Ludus::Graphics
{
	class GLContext
	{
	public:
		static void Init();
		static void EnableBlending();
		static void SetBlendAlpha();
	};

}
