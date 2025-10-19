#include <Pong/States/ScoreMenuState.h>

namespace Pong::States
{
	void ScoreMenuState::Init()
	{
		auto halfWidth = m_GameInfo.WindowOptions.Width * 0.5f;
		auto halfHeight = m_GameInfo.WindowOptions.Height * 0.5f;
		auto colorWhite = Ludus::Graphics::Colors::White;

		m_MenuItems.emplace_back("", Ludus::Math::Transform2D(0, { halfWidth, m_GameInfo.WindowOptions.Height - 150.0f }), colorWhite, colorWhite);
		m_MenuItems.emplace_back("New Game", Ludus::Math::Transform2D(0, { halfWidth, halfHeight }));
		m_MenuItems.emplace_back("Exit", Ludus::Math::Transform2D(0, { halfWidth, halfHeight - 100.0f }));
	}

	void ScoreMenuState::Reset()
	{ }

	GameState ScoreMenuState::HandleInput(Ludus::Platform::Input& input, float deltaTime)
	{
		if (input.GetKeyDown(Ludus::Platform::Key::Up) || input.GetKeyDown(Ludus::Platform::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 2 : 1;
		}

		if (input.GetKeyDown(Ludus::Platform::Key::Enter) || input.GetKeyDown(Ludus::Platform::Key::Space))
		{
			if (m_MenuIndex == 1)
			{
				m_PongInfo.Player1Score = 0;
				m_PongInfo.Player2Score = 0;

				return GameState::Playing;
			}
			else
			{
				return GameState::Exit;
			}
		}

		return GameState::Score;
	}

	GameState ScoreMenuState::Update(float deltaTime)
	{
		return GameState::Score;
	}

	void ScoreMenuState::Render(float deltaTime)
	{
		std::string winnerName = m_PongInfo.Player1Score > m_PongInfo.Player2Score ? "Player 1" : "Player 2";
		std::string scoreText = winnerName + " Won!";

		auto& header = m_MenuItems[0];
		m_GameInfo.Renderer.DrawText(header.Transform, scoreText, header.ActiveColor, header.HorizontalTextAlignment);

		for (size_t i = 1; i < m_MenuItems.size(); i++)
		{
			auto& menuItem = m_MenuItems[i];
			const auto color = m_MenuIndex == i ? menuItem.ActiveColor : menuItem.InactiveColor;

			m_GameInfo.Renderer.DrawText(
				menuItem.Transform,
				menuItem.MenuText,
				color,
				menuItem.HorizontalTextAlignment
			);
		}
	}
}
