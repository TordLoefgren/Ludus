#pragma once

#include <Pong/Core/Configuration.h>

namespace Pong::Models
{
	struct RenderData
	{
		int Width = 0;
		int Height = 0;

		float GetHalfWidth() const { return Width * 0.5f; }
		float GetHalfHeight() const { return Height * 0.5f; }
		int GetLineHeight() const { return Height / Pong::Core::Configuration::Defaults::NumMidLines; }
		float GetHalfLineHeight() const { return GetLineHeight() * 0.5f; }
		float GetHalfPaddleHeight() const { return Pong::Core::Configuration::Defaults::PaddleHeight * 0.5f; }
	};
}
