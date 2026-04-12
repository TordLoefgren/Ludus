#pragma once

#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Engine/Core/Enums/FlagSet.h>

namespace Ludus::Editor::Core
{
	struct ExecutionManager
	{
	public:
		Ludus::Editor::Core::ExecutionMode ExecutionMode = ExecutionMode::Stop;

		void Apply(Ludus::Engine::Core::Enums::FlagSet& executionFlags, Ludus::Editor::Core::ExecutionMode mode);

		void SetFlag(Ludus::Engine::Core::Enums::FlagSet& executionFlags, EditorExecutionFlags flag);
		void UnsetFlag(Ludus::Engine::Core::Enums::FlagSet& executionFlags, EditorExecutionFlags flag);
	};
}
