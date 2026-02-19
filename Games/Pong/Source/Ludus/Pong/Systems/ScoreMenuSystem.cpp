#include <Ludus/Pong/Systems/ScoreMenuSystem.h>

#include <optional>

namespace Ludus::Pong::Systems
{
	ScoreMenuSystem::ScoreMenuSystem(std::shared_ptr<Ludus::Pong::Core::GameInfo> gameInfo, std::shared_ptr<Ludus::Pong::Core::PongInfo> pongInfo)
		: m_GameInfo(std::move(gameInfo)), m_PongInfo(std::move(pongInfo))
	{ }

	void ScoreMenuSystem::OnAttachImpl()
	{
		m_MenuItems.emplace_back(-1, "", Ludus::Engine::Components::Transform2DComponent(-1, { 0.0f, 6.0f }, 0.06f), m_GameInfo->ActiveColor);
		m_MenuItems.emplace_back(-1, "New Game", Ludus::Engine::Components::Transform2DComponent(-1, { 0.0f, 0.0f }, 0.04f), m_GameInfo->InactiveColor);
		m_MenuItems.emplace_back(-1, "Exit", Ludus::Engine::Components::Transform2DComponent(-1, { 0.0f, -2.0f }, 0.04f), m_GameInfo->InactiveColor);
	}

	void ScoreMenuSystem::OnDetachImpl()
	{
		m_MenuItems.clear();
	}

	void ScoreMenuSystem::OnTransitionEnterImpl()
	{
		LUDUS_LOG_DEBUG("ENTERING SCORE MENU STATE");

		auto [width, height] = m_SystemContext->Window.GetFramebufferSize();

		m_GameInfo->Camera.SetViewport(width, height);
		m_GameInfo->Camera.SetOrthographicSize(10.0f);
		m_GameInfo->Camera.SetPosition({ 0.0f, 0.0f });

		std::string winnerName = m_PongInfo->Player1Score > m_PongInfo->Player2Score ? "Player 1" : "Player 2";
		std::string scoreText = winnerName + " Won!";
		m_MenuItems[0].MenuText = scoreText;

		auto& ecs = m_SystemContext->EntityComponentSystem;

		for (auto& item : m_MenuItems)
		{
			auto handle = ecs.AddEntity();
			ecs.AttachTransform(handle, item.Transform.Position, item.Transform.Scale);
			ecs.AttachText(handle, item.MenuText, item.Color, item.HorizontalTextAlignment);
			item.Handle = handle;
		}
	}

	void ScoreMenuSystem::OnTransitionExitImpl()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		for (auto& item : m_MenuItems)
		{
			ecs.DestroyEntity(item.Handle);
		}
	}

	void ScoreMenuSystem::UpdateImpl(float deltaTime)
	{
		auto& input = m_SystemContext->Input;

		if (input.GetKeyDown(Ludus::Engine::Windowing::Key::Up) || input.GetKeyDown(Ludus::Engine::Windowing::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 2 : 1;
		}

		if (input.GetKeyDown(Ludus::Engine::Windowing::Key::Enter) || input.GetKeyDown(Ludus::Engine::Windowing::Key::Space))
		{
			if (m_MenuIndex == 1)
			{
				m_PongInfo->Player1Score = 0;
				m_PongInfo->Player2Score = 0;

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

		m_SystemContext->RenderViews.RegisterFullscreen(std::nullopt, m_GameInfo->Camera, m_SystemContext->WindowRenderTarget);
	}
}
