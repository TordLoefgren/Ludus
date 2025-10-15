#pragma once

#include <vector>

#include <Ludus/Graphics/Color.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Platform/Key.h>

#include <Pong/Core/MenuItem.h>
#include <Pong/States/GameState.h>
#include <Pong/States/IPongState.h>

namespace Pong::States
{
	class MainMenuState : IPongState
	{
	private:
		int m_MenuIndex = 1;
		std::vector<Pong::Core::MenuItem> m_MenuItems;

	public:
		MainMenuState(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo) : IPongState(gameInfo, pongInfo) { }
		~MainMenuState() = default;

		void Init() override;
		void Reset() override;
		GameState HandleInput(Ludus::Platform::Input& input, float deltaTime) override;
		GameState Update(float deltaTime) override;
		void Render(float deltaTime) override;
	};
}
