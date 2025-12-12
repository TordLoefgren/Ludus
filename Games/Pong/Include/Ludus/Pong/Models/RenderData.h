#pragma once

#include <Ludus/Engine/Math/Vector2D.h>

#include <Ludus/Pong/Core/Configuration.h>

namespace Ludus::Pong::Models
{
	struct RenderData
	{
		float Width = 0;
		float Height = 0;

		Ludus::Engine::Math::Vector2D DefaultBallPosition;
		Ludus::Engine::Math::Vector2D DefaultPlayer1Position;
		Ludus::Engine::Math::Vector2D DefaultPlayer2Position;

		float GetHalfWidth() const { return Width * 0.5f; }
		float GetHalfHeight() const { return Height * 0.5f; }
		float GetLineWidth() const { return Width * 0.005f; }
		float GetLineHeight() const { return Height / Pong::Core::Configuration::Defaults::NumMidLines; }
		float GetHalfLineHeight() const { return GetLineHeight() * 0.5f; }
		float GetHalfPaddleHeight() const { return Pong::Core::Configuration::Defaults::PaddleHeight * 0.5f; }
	};
}
