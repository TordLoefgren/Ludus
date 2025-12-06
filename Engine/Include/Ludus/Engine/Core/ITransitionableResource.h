#pragma once

namespace Ludus::Engine::Core
{
	struct ITransitionableResource
	{
		virtual ~ITransitionableResource() = default;
		virtual void ApplyStateTransition() = 0;
	};
}
