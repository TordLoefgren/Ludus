#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Physics/Core/BodyType.h>
#include <Ludus/Physics/Core/Constants.h>

#include <Pong/Core/GameInfo.h>
#include <Pong/Core/GameState.h>
#include <Pong/Core/Layers.h>
#include <Pong/Core/PongInfo.h>
#include <Pong/Models/Entities.h>
#include <Pong/Models/Intents.h>
#include <Pong/Models/RenderData.h>
#include <Pong/Models/RuntimeData.h>

namespace Pong::Systems
{

	class PlayingSystem final : public Ludus::Core::ISystem
	{
		Pong::Core::GameInfo& m_GameInfo;
		Pong::Core::PongInfo& m_PongInfo;

		Pong::Models::Entities m_Entities;
		Pong::Models::Intents m_Intents;
		Pong::Models::RenderData m_RenderData;
		Pong::Models::RuntimeData m_RuntimeData;

		Ludus::Engine::LayerIndex m_LayerIndexBall, m_LayerIndexHorizontal, m_LayerIndexVertical, m_LayerIndexPlayer1, m_LayerIndexPlayer2;
		Ludus::Engine::LayerMask  m_LayerMaskBall, m_LayerMaskHorizontal, m_LayerMaskVertical, m_LayerMaskPlayer1, m_LayerMaskPlayer2;

		void Start();
		void Clear();

	public:
		PlayingSystem(Pong::Core::GameInfo& gameInfo, Pong::Core::PongInfo& pongInfo);

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void OnTransitionEnterImpl() override;
		virtual void OnTransitionExitImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
	};
}
