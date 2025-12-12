#include <Ludus/Pong/Systems/PauseMenuSystem.h>

namespace Ludus::Pong::Systems
{
	PauseMenuSystem::PauseMenuSystem(std::shared_ptr<Ludus::Pong::Core::GameInfo> gameInfo, std::shared_ptr<Ludus::Pong::Core::PongInfo> pongInfo)
		: m_GameInfo(std::move(gameInfo)), m_PongInfo(std::move(pongInfo))
	{ }

	void PauseMenuSystem::OnAttachImpl()
	{
		m_MenuItems.emplace_back(-1, "Pong", Ludus::Engine::Math::Transform2D(-1, { 0.0f, 6.0f }, 0.08f), m_GameInfo->ActiveColor);
		m_MenuItems.emplace_back(-1, "Continue", Ludus::Engine::Math::Transform2D(-1, { 0.0f, 2.0f }, 0.04f), m_GameInfo->InactiveColor);
		m_MenuItems.emplace_back(-1, "Exit", Ludus::Engine::Math::Transform2D(-1, { 0.0f, -1.0f }, 0.04f), m_GameInfo->InactiveColor);
	}

	void PauseMenuSystem::OnDetachImpl()
	{
		m_MenuItems.clear();
	}

	void PauseMenuSystem::OnTransitionEnterImpl()
	{
		LUDUS_LOG_DEBUG("ENTERING PAUSE MENU STATE");

		auto [width, height] = m_SystemContext->Window.GetFramebufferSize();

		m_GameInfo->Camera.SetViewport(width, height);
		m_GameInfo->Camera.SetOrthographicSize(10.0f);
		m_GameInfo->Camera.SetPosition({ 0.0f, 0.0f });

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

		if (input.GetKeyDown(Ludus::Engine::Platform::Key::Up) || input.GetKeyDown(Ludus::Engine::Platform::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 2 : 1;
		}

		if (input.GetKeyDown(Ludus::Engine::Platform::Key::Enter) || input.GetKeyDown(Ludus::Engine::Platform::Key::Space))
		{
			if (m_MenuIndex == 1)
			{
				auto& gameState = m_SystemContext->Resources.Get<Ludus::Engine::Core::State<Ludus::Pong::Core::GameState>>();
				gameState.TransitionTo(Ludus::Pong::Core::GameState::Playing);
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
				text->Color = i == m_MenuIndex ? m_GameInfo->ActiveColor : m_GameInfo->InactiveColor;
			}
		}

		m_SystemContext->RenderViews.RegisterFullscreen(m_GameInfo->Camera, m_SystemContext->WindowRenderTarget);
	}
}
