#include "pch.h"

#include <string>

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/LayoutContext.h>

#pragma once

namespace Ludus::UI::Context::LayoutContext
{
	void AlignTextToFramePadding()
	{
		ImGui::AlignTextToFramePadding();
	}

	void SameLine(float offsetFromStart, float spacing)
	{
		ImGui::SameLine(offsetFromStart, spacing);
	}

	void SetNextItemWidth(float width)
	{
		ImGui::SetNextItemWidth(width);
	}

	void Separator()
	{
		ImGui::Separator();
	}

	float CalculateItemWidth()
	{
		return ImGui::CalcItemWidth();
	}

	Ludus::Engine::Math::Vector2D CalculateTextSize(std::string text)
	{
		const auto size = ImGui::CalcTextSize(text.c_str());
		return { size.x, size.y };
	}

	void DrawRect(
		const Ludus::Engine::Math::Vector2D& position,
		const Ludus::Engine::Math::Vector2D& size,
		const Ludus::Engine::Graphics::Color color
	)
	{
		auto* drawList = ImGui::GetWindowDrawList();

		const auto windowPosition = ImGui::GetWindowPos();
		const ImVec2 min = { windowPosition.x + position.X, windowPosition.y + position.Y };
		const ImVec2 max = { min.x + size.X, min.y + size.Y };

		drawList->AddRectFilled(
			min,
			max,
			ImGui::ColorConvertFloat4ToU32({ color.R, color.G, color.B, color.A })
		);
	}
}
