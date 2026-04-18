#pragma once

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Types/DrawList.h>

namespace Ludus::UI::Types
{
	struct DrawList
	{
		void AddRectFilled(
			const Ludus::Engine::Math::Vector2D& min,
			const Ludus::Engine::Math::Vector2D& max,
			Ludus::Engine::Graphics::Color color,
			float rounding = 0.0f,
			Ludus::UI::Flags::Draw flags = Ludus::UI::Flags::Draw::None
		);

		void AddRect(
			const Ludus::Engine::Math::Vector2D& min,
			const Ludus::Engine::Math::Vector2D& max,
			Ludus::Engine::Graphics::Color color,
			float rounding = 0.0f,
			Ludus::UI::Flags::Draw flags = Ludus::UI::Flags::Draw::None
		);

		void AddText(
			const Ludus::Engine::Math::Vector2D& position,
			Ludus::Engine::Graphics::Color color,
			const char* text
		);
	};
}

