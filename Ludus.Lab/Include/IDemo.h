#pragma once

#include <Ludus/Platform/Input.h>

namespace Ludus::Lab
{
	class IDemo
	{
	public:
		virtual ~IDemo() { }
		virtual void Init() = 0;
		virtual void Update(float deltaTime, Ludus::Platform::Input& input) = 0;
	};
}
