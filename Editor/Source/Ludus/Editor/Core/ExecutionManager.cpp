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
		static constexpr uint32_t editorToolsEnabled = Ludus::Engine::Core::Mask(Ludus::Editor::Core::EditorExecutionFlags::EditorToolsEnabled);
		static constexpr uint32_t physicsEnabledFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled);
		static constexpr uint32_t simulatingFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::SimulationEnabled);

		switch (mode)
		{
			case Ludus::Editor::Core::ExecutionMode::Start:
				context.ExecutionFlags.Set(simulatingFlag | physicsEnabledFlag);
				context.ExecutionFlags.Unset(editorToolsEnabled);
				break;
			case Ludus::Editor::Core::ExecutionMode::Stop:
				context.ExecutionFlags.Set(editorToolsEnabled);
				context.ExecutionFlags.Unset(simulatingFlag | physicsEnabledFlag);
				break;
			case Ludus::Editor::Core::ExecutionMode::Pause:
				context.ExecutionFlags.Set(editorToolsEnabled);
				context.ExecutionFlags.Unset(simulatingFlag | physicsEnabledFlag);
				break;
		}
	}
}
