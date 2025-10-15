#pragma once

#include <Ludus/Platform/Input.h>

#include <Pong/Core/GameInfo.h>
#include <Pong/Core/PongInfo.h>
#include <Pong/States/GameState.h>

namespace Pong::States
{
	class IPongState
	{
	protected:
		Pong::Core::GameInfo& m_GameInfo;
		Pong::Core::PongInfo& m_PongInfo;

	public:
		IPongState(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo);
		virtual ~IPongState() { }

		virtual void Init() = 0;
		virtual void Reset() = 0;
		virtual GameState HandleInput(Ludus::Platform::Input& input, float deltaTime) = 0;
		virtual GameState Update(float deltaTime) = 0;
		virtual void Render(float deltaTime) = 0;
	};
}
