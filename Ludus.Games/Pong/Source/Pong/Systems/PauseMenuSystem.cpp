#include "Pong/Systems/PauseMenuSystem.h"

namespace Pong::Systems
{
	PauseMenuSystem::PauseMenuSystem(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo)
		: m_GameInfo(gameInfo), m_PongInfo(pongInfo)
	{ }

	void PauseMenuSystem::OnAttachImpl()
	{
		auto& options = m_SystemContext->Window.GetOptions();
		auto halfWidth = options.Width * 0.5f;
		auto halfHeight = options.Height * 0.5f;

		m_MenuItems.emplace_back(-1, "Pong", Ludus::Math::Transform2D(0, { halfWidth, options.Height - 150.0f }, 3.0f), m_GameInfo.ActiveColor);
		m_MenuItems.emplace_back(-1, "Continue", Ludus::Math::Transform2D(0, { halfWidth, halfHeight }), m_GameInfo.InactiveColor);
		m_MenuItems.emplace_back(-1, "Exit", Ludus::Math::Transform2D(0, { halfWidth, halfHeight - 100.0f }), m_GameInfo.InactiveColor);
	}

	void PauseMenuSystem::OnDetachImpl()
	{
		m_MenuItems.clear();
	}

	void PauseMenuSystem::OnTransitionEnterImpl()
	{
		LUDUS_LOG_DEBUG("ENTERING PAUSE MENU STATE");

		auto& ecs = m_SystemContext->EntityComponentSystem;

		for (auto& item : m_MenuItems)
		{
			auto handle = ecs.AddEntity();
			ecs.AttachTransform(handle, item.Transform.Position, item.Transform.Scale);
			ecs.AttachText(handle, item.MenuText, item.Color, item.HorizontalTextAlignment);
			item.Handle = handle;
		}
	}

	void PauseMenuSystem::OnTransitionExitImpl()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		for (auto& item : m_MenuItems)
		{
			ecs.DestroyEntity(item.Handle);
		}
	}

	void PauseMenuSystem::UpdateImpl(float deltaTime)
	{
		auto& input = m_SystemContext->Input;

		if (input.GetKeyDown(Ludus::Platform::Key::Up) || input.GetKeyDown(Ludus::Platform::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 2 : 1;
		}

		if (input.GetKeyDown(Ludus::Platform::Key::Enter) || input.GetKeyDown(Ludus::Platform::Key::Space))
		{
			if (m_MenuIndex == 1)
			{
				auto& gameState = m_SystemContext->Resources.Get<Ludus::Core::State<Pong::Core::GameState>>();
				gameState.TransitionTo(Pong::Core::GameState::Playing);
			}
			else
			{
				m_SystemContext->Window.SetWindowShouldClose();
			}
		}

		auto& ecs = m_SystemContext->EntityComponentSystem;
		for (int i = 1; i < m_MenuItems.size(); i++)
		{
			auto* text = ecs.Texts.TryGetByOwnerMutable(m_MenuItems[i].Handle);
			if (text)
			{
				text->Color = i == m_MenuIndex ? m_GameInfo.ActiveColor : m_GameInfo.InactiveColor;
			}
		}
	}
}
