#pragma once

namespace Ludus::Core
{
	struct ITransitionableResource
	{
		virtual ~ITransitionableResource() = default;
		virtual void ApplyStateTransition() = 0;
	};
}
