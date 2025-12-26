#include "pch.h"

#include <memory>

#include <Ludus/Editor/Core/EditorSystem.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>

namespace Ludus::Editor::Core
{
	EditorSystem::EditorSystem(Ludus::Editor::Core::EditorConfiguration editorOptions)
		: m_EditorContext(), m_EditorConfiguration(editorOptions), m_PanelRegistry()
	{ }

	void EditorSystem::AddViewport()
	{
		m_PanelRegistry.Register(std::make_unique<Ludus::Editor::Panels::ViewportPanel>());
	}

	void EditorSystem::HandleRequests()
	{
		if (m_EditorContext.State.Requests.AddViewport)
		{
			AddViewport();
		}

		m_EditorContext.State.Requests.Clear();
	}

	void Ludus::Editor::Core::EditorSystem::OnAttachImpl()
	{
		for (auto& factoryMethod : m_EditorConfiguration.PanelFactories)
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

		for (auto& panel : m_PanelRegistry.View())
		{
			if (!panel->Update(context))
			{
				m_PanelRegistry.ScheduleRemove(panel->GetHandle());
			}
		}

		m_PanelRegistry.ApplyRemovals();

		HandleRequests();
	}
}
