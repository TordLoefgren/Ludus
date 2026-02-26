#include "pch.h"

#include <Ludus/Engine/Windowing/Window.h>
#include <Ludus/UI/Systems/ImGuiSystem.h>

namespace Ludus::UI::Systems
{
	void ImGuiSystem::OnAttachImpl()
	{
		m_Backend.Initialize(m_SystemContext->Window.GetHandle());
	}

	void ImGuiSystem::OnDetachImpl()
	{
		m_Backend.Shutdown();
	}

	void ImGuiSystem::UpdateImpl(float deltaTime)
	{
		m_Backend.Begin();
	}

	void ImGuiSystem::RenderImpl()
	{
		m_Backend.End();
	}
}
