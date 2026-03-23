#include "pch.h"

#include <Ludus/Engine/Windowing/Window.h>
#include <Ludus/UI/Systems/ImGuiSystem.h>

namespace Ludus::UI::Systems
{
	ImGuiSystem::ImGuiSystem(GLFWwindow* window)
		: m_Window(window), m_Backend()
	{ }

	void ImGuiSystem::OnAttachImpl()
	{
		m_Backend.Initialize(m_Window);
	}

	void ImGuiSystem::OnDetachImpl()
	{
		m_Backend.Shutdown();
	}

	void ImGuiSystem::BeginFrameImpl()
	{
		m_Backend.Begin();
	}

	void ImGuiSystem::EndFrameImpl()
	{
		m_Backend.End();
	}
}
