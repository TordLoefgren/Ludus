#include "pch.h"

#include <memory>
#include <variant>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/EditorSystem.h>

namespace Ludus::Editor::Core
{
	namespace
	{
		template<typename TCommand>
		void DelegateCommands(std::vector<TCommand>& stateCommands, EditorContext& editorContext, Ludus::Engine::Core::SystemContext& systemContext, Ludus::Editor::Panels::PanelRegistry& panelRegistry)
		{
			std::vector<TCommand> commands;
			commands.swap(stateCommands);

			Ludus::Editor::Commands::CommandContext context { editorContext, systemContext, panelRegistry };

			for (const auto& command : commands)
			{
				Ludus::Editor::Commands::Execute(command, context);
			}
		}
	}

	EditorSystem::EditorSystem(Ludus::Editor::Core::EditorConfiguration editorOptions)
		: m_EditorContext(), m_EditorConfiguration(editorOptions), m_PanelRegistry()
	{ }

	void EditorSystem::DelegateUICommands()
	{
		DelegateCommands<Ludus::Editor::Commands::UICommand>(m_EditorContext.State.Commands.PendingCommands.UICommands, m_EditorContext, *m_SystemContext, m_PanelRegistry);
	}

	void EditorSystem::DelegateEditCommands()
	{
		DelegateCommands<Ludus::Editor::Commands::EditCommand>(m_EditorContext.State.Commands.PendingCommands.EditCommands, m_EditorContext, *m_SystemContext, m_PanelRegistry);
	}

	void EditorSystem::DelegateRequestCommands()
	{
		DelegateCommands<Ludus::Editor::Commands::RequestCommand>(m_EditorContext.State.Commands.PendingCommands.RequestCommands, m_EditorContext, *m_SystemContext, m_PanelRegistry);
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
		DelegateUICommands();

		Ludus::Editor::Panels::PanelContext context { *m_SystemContext, m_EditorContext, m_ActivePanelState, deltaTime };

		for (const auto& panel : m_PanelRegistry.View())
		{
			if (!panel->Update(context))
			{
				m_PanelRegistry.ScheduleRemove(panel->GetHandle());
			}
		}

		m_PanelRegistry.ApplyRemovals();

		if (auto commands = m_EditorContext.State.Dialogs.Update())
		{
			m_EditorContext.State.Commands.EnqueueCommands(std::move(commands.value()));
		}

		DelegateEditCommands();
		DelegateRequestCommands();

		m_EditorContext.State.Commands.ClearEntityReferences();
	}
}
