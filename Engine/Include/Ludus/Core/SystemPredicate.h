#pragma once

#include <functional>

#include <Ludus/Core/State.h>
#include <Ludus/Core/SystemContext.h>

namespace Ludus::Core
{
	using SystemPredicate = std::function<bool(const SystemContext&)>;

	template<typename TState>
	SystemPredicate RunIfInState(TState expectedState)
	{
		return [expectedState](const SystemContext& context)
			{
				auto& state = context.Resources.Get<State<TState>>();
				return state.Current == expectedState;
			};
	}
}
