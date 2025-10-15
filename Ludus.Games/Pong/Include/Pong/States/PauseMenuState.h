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
	class PauseMenuState : IPongState
	{
	private:
		int m_MenuIndex = 1;
		std::vector<Pong::Core::MenuItem> m_MenuItems;

	public:
		PauseMenuState(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo) : IPongState(gameInfo, pongInfo) { }
		~PauseMenuState() = default;

		void Init() override;
		void Reset() override;
		GameState HandleInput(Ludus::Platform::Input& input, float deltaTime) override;
		GameState Update(float deltaTime) override;
		void Render(float deltaTime) override;
	};
}
