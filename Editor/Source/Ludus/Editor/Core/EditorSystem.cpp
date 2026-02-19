#include "pch.h"

#include <memory>
#include <variant>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/RequestCommandVisitor.h>
#include <Ludus/Editor/Core/EditorSystem.h>

namespace Ludus::Editor::Core
{
	EditorSystem::EditorSystem(Ludus::Editor::Core::EditorConfiguration editorOptions)
		: m_EditorContext(), m_EditorConfiguration(editorOptions), m_PanelRegistry()
	{ }

	void EditorSystem::HandleRequestCommands()
	{
		std::vector <Ludus::Editor::Commands::RequestCommand> commands;
		commands.swap(m_EditorContext.State.RequestCommands);

		for (const auto& command : commands)
		{
			Ludus::Editor::Commands::CommandContext context { m_EditorContext, *m_SystemContext, m_PanelRegistry };
			std::visit(Ludus::Editor::Commands::RequestCommandVisitor { context }, command.Data);
		}
	}

	void Ludus::Editor::Core::EditorSystem::OnAttachImpl()
	{
		for (const auto& factoryMethod : m_EditorConfiguration.PanelFactories)
		{
			m_PanelRegistry.Register(factoryMethod());
		}
	}

	void Ludus::Editor::Core::EditorSystem::OnDetachImpl()
	{
		m_PanelRegistry.Clear();
	}

	void Ludus::Editor::Core::EditorSystem::UpdateImpl(float deltaTime)
	{
		Ludus::Editor::Panels::PanelContext context { *m_SystemContext, m_EditorContext, m_ActivePanelState, deltaTime };

		for (const auto& panel : m_PanelRegistry.View())
		{
			if (!panel->Update(context))
			{
				m_PanelRegistry.ScheduleRemove(panel->GetHandle());
			}
		}

		m_PanelRegistry.ApplyRemovals();

		HandleRequestCommands();
	}
}
