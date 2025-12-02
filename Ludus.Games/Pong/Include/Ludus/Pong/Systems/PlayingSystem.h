#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Physics/Core/BodyType.h>
#include <Ludus/Physics/Core/Constants.h>

#include <Ludus/Pong/Core/GameInfo.h>
#include <Ludus/Pong/Core/GameState.h>
#include <Ludus/Pong/Core/Layers.h>
#include <Ludus/Pong/Core/PongInfo.h>
#include <Ludus/Pong/Models/Entities.h>
#include <Ludus/Pong/Models/Intents.h>
#include <Ludus/Pong/Models/RenderData.h>
#include <Ludus/Pong/Models/RuntimeData.h>

namespace Ludus::Pong::Systems
{

	class PlayingSystem final : public Ludus::Core::ISystem
	{
		Ludus::Pong::Core::GameInfo& m_GameInfo;
		Ludus::Pong::Core::PongInfo& m_PongInfo;

		Ludus::Pong::Models::Entities m_Entities;
		Ludus::Pong::Models::Intents m_Intents;
		Ludus::Pong::Models::RenderData m_RenderData;
		Ludus::Pong::Models::RuntimeData m_RuntimeData;

		Ludus::Engine::LayerIndex m_LayerIndexBall, m_LayerIndexHorizontal, m_LayerIndexVertical, m_LayerIndexPlayer1, m_LayerIndexPlayer2;
		Ludus::Engine::LayerMask  m_LayerMaskBall, m_LayerMaskHorizontal, m_LayerMaskVertical, m_LayerMaskPlayer1, m_LayerMaskPlayer2;

		void Start();
		void Clear();

	public:
		PlayingSystem(Ludus::Pong::Core::GameInfo& gameInfo, Ludus::Pong::Core::PongInfo& pongInfo);

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void OnTransitionEnterImpl() override;
		virtual void OnTransitionExitImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
	};
}
