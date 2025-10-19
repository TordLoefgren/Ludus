#include <Pong/States/MainMenuState.h>

namespace Pong::States
{
	void MainMenuState::Init()
	{
		auto halfWidth = m_GameInfo.WindowOptions.Width * 0.5f;
		auto halfHeight = m_GameInfo.WindowOptions.Height * 0.5f;
		auto colorWhite = Ludus::Graphics::Colors::White;

		m_MenuItems.emplace_back("Pong", Ludus::Math::Transform2D(0, { halfWidth, m_GameInfo.WindowOptions.Height - 150.0f }, 3.0f), colorWhite, colorWhite);
		m_MenuItems.emplace_back("Single Player", Ludus::Math::Transform2D(0, { halfWidth, halfHeight }));
		m_MenuItems.emplace_back("Multiplayer", Ludus::Math::Transform2D(0, { halfWidth, halfHeight - 100.0f }));
		m_MenuItems.emplace_back("Exit", Ludus::Math::Transform2D(0, { halfWidth, halfHeight - 200.0f }));
	}

	void MainMenuState::Reset()
	{ }

	GameState MainMenuState::HandleInput(Ludus::Platform::Input& input, float deltaTime)
	{
		if (input.GetKeyDown(Ludus::Platform::Key::Up))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 3 : m_MenuIndex - 1;
		}

		if (input.GetKeyDown(Ludus::Platform::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 3 ? 1 : m_MenuIndex + 1;
		}

		if (input.GetKeyDown(Ludus::Platform::Key::Enter) || input.GetKeyDown(Ludus::Platform::Key::Space))
		{
			auto state = GameState::MainMenu;

			switch (m_MenuIndex)
			{
				case 1:
					m_PongInfo.IsMultiplayer = false;
					state = GameState::Playing;
					break;
				case 2:
					m_PongInfo.IsMultiplayer = true;
					state = GameState::Playing;
					break;
				case 3:
					state = GameState::Exit;
					break;
			}

			return state;
		}

		return GameState::MainMenu;
	}

	GameState MainMenuState::Update(float deltaTime)
	{
		return GameState::MainMenu;
	}

	void MainMenuState::Render(float deltaTime)
	{
		auto& header = m_MenuItems[0];
		m_GameInfo.Renderer.DrawText(header.Transform, header.MenuText, header.ActiveColor, header.HorizontalTextAlignment);

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