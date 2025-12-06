#include "Ludus/Engine/Core/ImGuiSystem.h"

namespace Ludus::Engine::Core
{
	void ImGuiSystem::OnAttachImpl()
	{
		m_Context.Initialize(m_SystemContext->Window.GetHandle());
	}

	void ImGuiSystem::OnDetachImpl()
	{
		m_Context.Shutdown();
	}

	void ImGuiSystem::UpdateImpl(float deltaTime)
	{
		m_Context.Begin();
	}

	void ImGuiSystem::RenderImpl()
	{
		m_Context.End();
	}
}
