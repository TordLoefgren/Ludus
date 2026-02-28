#pragma once

#include <Ludus/Editor/Core/ExecutionMode.h>

namespace Ludus::Engine::Core
{
	struct SystemContext;
}

namespace Ludus::Editor::Core
{
	struct ExecutionManager
	{
	public:
		Ludus::Editor::Core::ExecutionMode ExecutionMode = ExecutionMode::Stop;

		void Apply(Ludus::Engine::Core::SystemContext& context, Ludus::Editor::Core::ExecutionMode mode);
	};
}
