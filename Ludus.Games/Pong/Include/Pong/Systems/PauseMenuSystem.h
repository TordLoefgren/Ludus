#pragma once

#include <Ludus/Core/ISystem.h>

#include <Pong/Core/GameInfo.h>
#include <Pong/Core/GameState.h>
#include <Pong/Core/MenuItem.h>
#include <Pong/Core/PongInfo.h>

namespace Pong::Systems
{

	class PauseMenuSystem final : public Ludus::Core::ISystem
	{
		Pong::Core::GameInfo& m_GameInfo;
		Pong::Core::PongInfo& m_PongInfo;

		int m_MenuIndex = 1;
		std::vector<Pong::Core::MenuItem> m_MenuItems;

	public:
		PauseMenuSystem(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo);

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void OnTransitionEnterImpl() override;
		virtual void OnTransitionExitImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
	};
}
