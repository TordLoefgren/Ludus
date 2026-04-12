#include "pch.h"

#include <cstdint>

#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/ExecutionManager.h>
#include <Ludus/Engine/Core/Enums/EnumBits.h>
#include <Ludus/Engine/Core/Enums/FlagSet.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>

namespace Ludus::Editor::Core
{
	void ExecutionManager::Apply(Ludus::Engine::Core::Enums::FlagSet& executionFlags, Ludus::Editor::Core::ExecutionMode mode)
	{
		static constexpr uint32_t editorToolsFlag = Ludus::Engine::Core::Enums::ToUnderlyingType(Ludus::Editor::Core::EditorExecutionFlags::EditorToolsEnabled);
		static constexpr uint32_t physicsFlag = Ludus::Engine::Core::Enums::ToUnderlyingType(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled);
		static constexpr uint32_t scriptingFlag = Ludus::Engine::Core::Enums::ToUnderlyingType(Ludus::Engine::Core::ExecutionFlags::ScriptingEnabled);
		static constexpr uint32_t simulationFlag = Ludus::Engine::Core::Enums::ToUnderlyingType(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled);
		static constexpr uint32_t simulationSessionFlag = Ludus::Engine::Core::Enums::ToUnderlyingType(Ludus::Engine::Core::ExecutionFlags::SimulationSessionEnabled);
		static constexpr uint32_t startFlags = physicsFlag | scriptingFlag | simulationFlag | simulationSessionFlag;
		static constexpr uint32_t pauseFlags = scriptingFlag | simulationSessionFlag;

		switch (mode)
		{
			case Ludus::Editor::Core::ExecutionMode::Start:
				executionFlags.Set(startFlags);
				executionFlags.Unset(editorToolsFlag);
				break;
			case Ludus::Editor::Core::ExecutionMode::Stop:
				executionFlags.Set(editorToolsFlag);
				executionFlags.Unset(startFlags);
				break;
			case Ludus::Editor::Core::ExecutionMode::Pause:
				executionFlags.Set(editorToolsFlag);
				executionFlags.Set(pauseFlags);
				executionFlags.Unset(physicsFlag | simulationFlag);
				break;
		}
	}

	void ExecutionManager::SetFlag(Ludus::Engine::Core::Enums::FlagSet& executionFlags, EditorExecutionFlags flag)
	{
		executionFlags.Set(flag);
	}

	void ExecutionManager::UnsetFlag(Ludus::Engine::Core::Enums::FlagSet& executionFlags, EditorExecutionFlags flag)
	{
		executionFlags.Unset(flag);
	}
}
