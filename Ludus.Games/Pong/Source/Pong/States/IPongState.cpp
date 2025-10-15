#include <Pong/States/IPongState.h>

namespace Pong::States
{
	IPongState::IPongState(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo)
		: m_GameInfo(gameInfo), m_PongInfo(pongInfo)
	{ }
}
