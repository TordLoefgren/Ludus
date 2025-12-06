#include <Ludus/Pong/Systems/ScoreMenuSystem.h>

namespace Ludus::Pong::Systems
{
	ScoreMenuSystem::ScoreMenuSystem(Ludus::Pong::Core::GameInfo& gameInfo, Ludus::Pong::Core::PongInfo& pongInfo)
		: m_GameInfo(gameInfo), m_PongInfo(pongInfo)
	{ }

	void ScoreMenuSystem::OnAttachImpl()
	{
		auto& options = m_SystemContext->Window.GetOptions();
		auto halfWidth = options.Width * 0.5f;
		auto halfHeight = options.Height * 0.5f;
		auto colorWhite = Ludus::Engine::Graphics::Colors::White;

		m_MenuItems.emplace_back(-1, "", Ludus::Engine::Math::Transform2D(0, { halfWidth, options.Height - 150.0f }), m_GameInfo.ActiveColor);
		m_MenuItems.emplace_back(-1, "New Game", Ludus::Engine::Math::Transform2D(0, { halfWidth, halfHeight }), m_GameInfo.InactiveColor);
		m_MenuItems.emplace_back(-1, "Exit", Ludus::Engine::Math::Transform2D(0, { halfWidth, halfHeight - 100.0f }), m_GameInfo.InactiveColor);
	}

	void ScoreMenuSystem::OnDetachImpl()
	{
		m_MenuItems.clear();
	}

	void ScoreMenuSystem::OnTransitionEnterImpl()
	{
		LUDUS_LOG_DEBUG("ENTERING SCORE MENU STATE");

		std::string winnerName = m_PongInfo.Player1Score > m_PongInfo.Player2Score ? "Player 1" : "Player 2";
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

		if (input.GetKeyDown(Ludus::Engine::Platform::Key::Up) || input.GetKeyDown(Ludus::Engine::Platform::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 2 : 1;
		}

		if (input.GetKeyDown(Ludus::Engine::Platform::Key::Enter) || input.GetKeyDown(Ludus::Engine::Platform::Key::Space))
		{
			if (m_MenuIndex == 1)
			{
				m_PongInfo.Player1Score = 0;
				m_PongInfo.Player2Score = 0;

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
				text->Color = i == m_MenuIndex ? m_GameInfo.ActiveColor : m_GameInfo.InactiveColor;
			}
		}
	}
}
