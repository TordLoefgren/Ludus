#include "pch.h"

#include <string>

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Widgets/Input.h>

namespace Ludus::UI::Widgets
{
	namespace
	{
		ImVec4 ToImVec4(const Ludus::Engine::Graphics::Color& color)
		{
			return ImVec4(color.R, color.G, color.B, color.A);
		}

		ImVec4 ScaleRgb(ImVec4 color, const float factor)
		{
			color.x *= factor;
			color.y *= factor;
			color.z *= factor;

			return color;
		}
	}

	bool DragInt(const std::string& label, int* value, float speed)
	{
		return ImGui::DragInt(label.c_str(), value, speed);
	}

	bool DragFloat(const std::string& label, float* value, float speed)
	{
		return ImGui::DragFloat(label.c_str(), value, speed, 0.0f, 0.0f, "%.2f");
	}

	bool DragFloatColoredButton(const char* label, float* value, const Ludus::Engine::Graphics::Color& color, const float speed)
	{
		const ImVec4 normal = ToImVec4(color);
		const ImVec4 hovered = ScaleRgb(normal, 1.10f);
		const ImVec4 active = ScaleRgb(normal, 0.90f);

		ImGui::PushStyleColor(ImGuiCol_Button, normal);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);

		ImGui::PushTabStop(false);
		const bool pressed = ImGui::Button(label);
		const bool isActive = ImGui::IsItemActive();
		ImGui::PopTabStop();

		ImGui::PopStyleColor(3);

		if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			*value += ImGui::GetIO().MouseDelta.x * speed;
			return true;
		}

		return pressed;
	}

	bool InputInt(const std::string& label, int* value, int step, int step_fast, Ludus::UI::Flags::InputText flags)
	{
		return ImGui::InputInt(label.c_str(), value, step, step_fast, static_cast<int>(flags));
	}

	bool InputUInt8(const char* label, uint8_t* value)
	{
		auto step = 1u;
		return ImGui::InputScalar(label, ImGuiDataType_U8, value, &step, nullptr, "%u");
	}

	bool InputFloat(const std::string& label, float* value, float step, float step_fast, const char* format, Ludus::UI::Flags::InputText flags)
	{
		return ImGui::InputFloat(label.c_str(), value, step, step_fast, format, static_cast<int>(flags));
	}

	bool InputText(const std::string& label, std::string& text, Ludus::UI::Flags::InputText flags)
	{
		return ImGui::InputText(label.c_str(), &text, static_cast<int>(flags));
	}
}
