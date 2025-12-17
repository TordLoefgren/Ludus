#include "pch.h"

#include <string>

#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Widgets/Input.h>

namespace Ludus::UI::Widgets
{
	bool DragInt(const std::string& label, int* value, float speed)
	{
		return ImGui::DragInt(label.c_str(), value, speed);
	}

	bool DragFloat(const std::string& label, float* value, float speed)
	{
		return ImGui::DragFloat(label.c_str(), value, speed, 0.0f, 0.0f, "%.2f");
	}

	bool DragFloatLabelButton(const char* label, float* value, float speed)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

		const auto _ = ImGui::Button(label);
		const auto active = ImGui::IsItemActive();

		ImGui::PopStyleColor(3);

		if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			*value += ImGui::GetIO().MouseDelta.x * speed;
			return true;
		}

		return false;
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
