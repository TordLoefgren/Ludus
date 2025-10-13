#pragma once

namespace Ludus::Lab
{
	class IDemo
	{
	public:
		virtual ~IDemo() { }
		virtual void Init() = 0;
		virtual void Update() = 0;
	};
}
