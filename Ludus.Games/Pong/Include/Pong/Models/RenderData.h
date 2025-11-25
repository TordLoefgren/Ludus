#pragma once

#include <Ludus/Math/Vector2D.h>

#include <Pong/Core/Configuration.h>

namespace Pong::Models
{
	struct RenderData
	{
		int Width = 0;
		int Height = 0;

		Ludus::Math::Vector2D DefaultBallPosition;
		Ludus::Math::Vector2D DefaultPlayer1Position;
		Ludus::Math::Vector2D DefaultPlayer2Position;

		float GetHalfWidth() const { return Width * 0.5f; }
		float GetHalfHeight() const { return Height * 0.5f; }
		float GetLineWidth() const { return Width * 0.005f; }
		int GetLineHeight() const { return Height / Pong::Core::Configuration::Defaults::NumMidLines; }
		float GetHalfLineHeight() const { return GetLineHeight() * 0.5f; }
		float GetHalfPaddleHeight() const { return Pong::Core::Configuration::Defaults::PaddleHeight * 0.5f; }
	};
}
