#pragma once

#include <Ludus/Engine/GameObject.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Engine/Random.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Math/Numeric.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/CollisionSystem2D.h>
#include <Ludus/Physics/Constants.h>
#include <Ludus/Platform/Input.h>
#include <Ludus/Platform/Key.h>

#include <Pong/Core/Configuration.h>
#include <Pong/Core/Layers.h>
#include <Pong/Models/Entities.h>
#include <Pong/Models/Intents.h>
#include <Pong/Models/RenderData.h>
#include <Pong/Models/RuntimeData.h>
#include <Pong/States/GameState.h>
#include <Pong/States/IPongState.h>

namespace Pong::States
{
	class PlayingState : IPongState
	{
	private:
		Pong::Models::Entities m_Entities;
		Pong::Models::Intents m_Intents;
		Pong::Models::RenderData m_RenderData;
		Pong::Models::RuntimeData m_RuntimeData;

		Ludus::Engine::LayerIndex m_LayerIndexBall, m_LayerIndexHorizontal, m_LayerIndexVertical, m_LayerIndexPlayer1, m_LayerIndexPlayer2;
		Ludus::Engine::LayerMask  m_LayerMaskBall, m_LayerMaskHorizontal, m_LayerMaskVertical, m_LayerMaskPlayer1, m_LayerMaskPlayer2;

		void Start();
		void Clear();

	public:
		PlayingState(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo) : IPongState(gameInfo, pongInfo) { }
		~PlayingState() = default;

		void Init() override;
		void Reset() override;
		GameState HandleInput(Ludus::Platform::Input& input, float deltaTime) override;
		GameState Update(float deltaTime) override;
		void Render(float deltaTime) override;
	};
}
