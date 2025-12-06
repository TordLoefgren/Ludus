#pragma once

#include <optional>

#include <Ludus/Engine/Core/ITransitionableResource.h>

namespace Ludus::Engine::Core
{
	// Inspired by the Bevy engine: https://bevy-cheatbook.github.io/programming/states.html

	template<typename TState>
	struct State : ITransitionableResource
	{
		TState Current;

	private:
		std::optional<TState> m_Next;

	public:
		explicit State(TState state) : Current(state) { }

		void TransitionTo(TState next) { m_Next = next; }

		void ApplyStateTransition() override
		{
			if (m_Next.has_value())
			{
				Current = *m_Next;
				m_Next.reset();
			}
		}
	};
}
