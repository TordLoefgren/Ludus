#include "pch.h"

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Types/DrawList.h>

namespace Ludus::UI::Types
{
	void DrawList::AddRectFilled(
		const Ludus::Engine::Math::Vector2D& min,
		const Ludus::Engine::Math::Vector2D& max,
		Ludus::Engine::Graphics::Color color,
		float rounding,
		Ludus::UI::Flags::Draw flags
	)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(
			{ min.X, min.Y },
			{ max.X, max.Y },
			ImGui::ColorConvertFloat4ToU32({ color.R, color.G, color.B, color.A }),
			rounding,
			static_cast<int>(flags)
		);
	}

	void DrawList::AddRect(
		const Ludus::Engine::Math::Vector2D& min,
		const Ludus::Engine::Math::Vector2D& max,
		Ludus::Engine::Graphics::Color color,
		float rounding,
		Ludus::UI::Flags::Draw flags
	)
	{
		ImGui::GetWindowDrawList()->AddRect(
			{ min.X, min.Y },
			{ max.X, max.Y },
			ImGui::ColorConvertFloat4ToU32({ color.R, color.G, color.B, color.A }),
			rounding,
			static_cast<int>(flags)
		);
	}

	void DrawList::AddText(
		const Ludus::Engine::Math::Vector2D& position,
		Ludus::Engine::Graphics::Color color,
		const char* text
	)
	{
		ImGui::GetWindowDrawList()->AddText(
			{ position.X, position.Y },
			ImGui::ColorConvertFloat4ToU32({ color.R, color.G, color.B, color.A }),
			text
		);
	}
}
