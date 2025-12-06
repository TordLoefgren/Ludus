#pragma once

#include <Ludus/Engine/Core/ISystem.h>

#include <Ludus/Pong/Core/GameInfo.h>
#include <Ludus/Pong/Core/GameState.h>
#include <Ludus/Pong/Core/MenuItem.h>
#include <Ludus/Pong/Core/PongInfo.h>

namespace Ludus::Pong::Systems
{

	class PauseMenuSystem final : public Ludus::Engine::Core::ISystem
	{
		Ludus::Pong::Core::GameInfo& m_GameInfo;
		Ludus::Pong::Core::PongInfo& m_PongInfo;

		int m_MenuIndex = 1;
		std::vector<Ludus::Pong::Core::MenuItem> m_MenuItems;

	public:
		PauseMenuSystem(Ludus::Pong::Core::GameInfo& gameInfo, Ludus::Pong::Core::PongInfo& pongInfo);

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void OnTransitionEnterImpl() override;
		virtual void OnTransitionExitImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
	};
}
