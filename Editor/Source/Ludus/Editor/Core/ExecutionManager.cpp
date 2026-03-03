#include "pch.h"

#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/ExecutionManager.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/FlagSet.h>
#include <Ludus/Engine/Core/Mask.h>
#include <Ludus/Engine/Core/SystemContext.h>

namespace Ludus::Editor::Core
{
	void ExecutionManager::Apply(Ludus::Engine::Core::SystemContext& context, Ludus::Editor::Core::ExecutionMode mode)
	{
		static constexpr uint32_t editorToolsFlag = Ludus::Engine::Core::Mask(Ludus::Editor::Core::EditorExecutionFlags::EditorToolsEnabled);
		static constexpr uint32_t physicsFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled);
		static constexpr uint32_t scriptingFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::ScriptingEnabled);
		static constexpr uint32_t simulationFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled);
		static constexpr uint32_t startFlags = physicsFlag | scriptingFlag | simulationFlag;
		static constexpr uint32_t pauseFlags = physicsFlag | scriptingFlag;

		switch (mode)
		{
			case Ludus::Editor::Core::ExecutionMode::Start:
				context.ExecutionFlags.Set(startFlags);
				context.ExecutionFlags.Unset(editorToolsFlag);
				break;
			case Ludus::Editor::Core::ExecutionMode::Stop:
				context.ExecutionFlags.Set(editorToolsFlag);
				context.ExecutionFlags.Unset(startFlags);
				break;
			case Ludus::Editor::Core::ExecutionMode::Pause:
				context.ExecutionFlags.Set(editorToolsFlag);
				context.ExecutionFlags.Set(pauseFlags);
				context.ExecutionFlags.Unset(simulationFlag);
				break;
		}
	}
}
