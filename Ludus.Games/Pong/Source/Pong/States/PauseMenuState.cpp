#include <Pong/States/PauseMenuState.h>

namespace Pong::States
{
	void PauseMenuState::Init()
	{
		auto halfWidth = m_GameInfo.WindowOptions.Width * 0.5f;
		auto halfHeight = m_GameInfo.WindowOptions.Height * 0.5f;
		auto colorWhite = Ludus::Graphics::Colors::White;

		m_MenuItems.emplace_back("Pong", Ludus::Math::Transform2D(0, { halfWidth - 175.0f, m_GameInfo.WindowOptions.Height - 150.0f }, 3.0f), colorWhite, colorWhite);
		m_MenuItems.emplace_back("Continue", Ludus::Math::Transform2D(0, { halfWidth - 80.0f, halfHeight }));
		m_MenuItems.emplace_back("Exit", Ludus::Math::Transform2D(0, { halfWidth - 30.0f, halfHeight - 100.0f }));
	}

	void PauseMenuState::Reset()
	{ }

	GameState PauseMenuState::HandleInput(Ludus::Platform::Input& input, float deltaTime)
	{
		if (input.GetKeyDown(Ludus::Platform::Key::Up) || input.GetKeyDown(Ludus::Platform::Key::Down))
		{
			m_MenuIndex = m_MenuIndex == 1 ? 2 : 1;
		}

		if (input.GetKeyDown(Ludus::Platform::Key::Enter) || input.GetKeyDown(Ludus::Platform::Key::Space))
		{
			if (m_MenuIndex == 1)
			{
				return GameState::Playing;
			}
			else
			{
				return GameState::Exit;
			}
		}

		return GameState::Paused;
	}

	GameState PauseMenuState::Update(float deltaTime)
	{
		return GameState::Paused;
	}

	void PauseMenuState::Render(float deltaTime)
	{
		auto& header = m_MenuItems[0];
		m_GameInfo.Renderer.DrawText(header.Transform, header.MenuText, header.ActiveColor);

		for (size_t i = 1; i < m_MenuItems.size(); i++)
		{
			auto& menuItem = m_MenuItems[i];
			const auto color = m_MenuIndex == i ? menuItem.ActiveColor : menuItem.InactiveColor;

			m_GameInfo.Renderer.DrawText(
				menuItem.Transform,
				menuItem.MenuText,
				color
			);
		}
	}
}