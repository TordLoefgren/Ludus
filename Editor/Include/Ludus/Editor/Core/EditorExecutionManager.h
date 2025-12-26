#pragma once

#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Mask.h>
#include <Ludus/Engine/Core/SystemContext.h>

namespace Ludus::Editor::Core
{
	struct EditorExecutionManager
	{
	private:
		static constexpr uint32_t m_EditorToolsEnabled = Ludus::Engine::Core::Mask(Ludus::Editor::Core::EditorExecutionFlags::EditorToolsEnabled);
		static constexpr uint32_t m_PhysicsEnabledFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::PhysicsEnabled);
		static constexpr uint32_t m_SimulatingFlag = Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::Simulating);

	public:
		void Apply(Ludus::Engine::Core::SystemContext& context, Ludus::Editor::Core::ExecutionMode mode)
		{
			switch (mode)
			{
			case Ludus::Editor::Core::ExecutionMode::Start:
				context.ExecutionFlags.Set(m_SimulatingFlag | m_PhysicsEnabledFlag);
				context.ExecutionFlags.Unset(m_EditorToolsEnabled);
				break;
			case Ludus::Editor::Core::ExecutionMode::Stop:
				context.ExecutionFlags.Set(m_EditorToolsEnabled);
				context.ExecutionFlags.Unset(m_SimulatingFlag | m_PhysicsEnabledFlag);
				break;
			case Ludus::Editor::Core::ExecutionMode::Pause:
				context.ExecutionFlags.Set(m_EditorToolsEnabled);
				context.ExecutionFlags.Unset(m_SimulatingFlag | m_PhysicsEnabledFlag);
				break;
			}
		}
	};
}
