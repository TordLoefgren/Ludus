#pragma once

#include <initializer_list>
#include <string>
#include <string_view>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <Ludus/UI/Utilities.h>

namespace Ludus::UI::Widgets
{
	inline void Text(std::string_view text)
	{
		ImGui::TextUnformatted(text.data(), text.data() + text.size());
	}

	inline void Text(const char* text)
	{
		ImGui::TextUnformatted(text);
	}

	inline bool DragInt(const std::string& label, int* value, float speed = 1.0f)
	{
		return ImGui::DragInt(label.c_str(), value, speed);
	}

	inline bool DragFloat(const std::string& label, float* value, float speed = 1.0f)
	{
		return ImGui::DragFloat(label.c_str(), value, speed, 0.0f, 0.0f, "%.2f");
	}

	inline bool DragFloatLabelButton(const char* label, float* value, float speed = 0.1f)
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

	inline bool InputInt(const std::string& label, int* value, int step = 0, int step_fast = 0, ImGuiInputTextFlags flags = 0)
	{
		return ImGui::InputInt(label.c_str(), value, step, step_fast, flags);
	}

	inline bool InputUInt8(const char* label, uint8_t* value)
	{
		auto step = 1u;
		return ImGui::InputScalar(label, ImGuiDataType_U8, value, &step, nullptr, "%u");
	}

	inline bool InputFloat(const std::string& label, float* value, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.2f", ImGuiInputTextFlags flags = 0)
	{
		return ImGui::InputFloat(label.c_str(), value, step, step_fast, format, flags);
	}

	inline bool InputText(const std::string& label, std::string& text, ImGuiInputTextFlags flags = 0)
	{
		return ImGui::InputText(label.c_str(), &text, flags);
	}

	inline bool ColorEdit4(const std::string& label, float color[4], ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs)
	{
		return ImGui::ColorEdit4(label.c_str(), color, flags);
	}

	struct CheckboxItem
	{
		std::string Label;
		bool IsChecked = false;
	};

	inline bool Checkbox(const std::string& label, bool* value)
	{
		return ImGui::Checkbox(label.c_str(), value);
	}

	inline bool Checkbox(CheckboxItem& item)
	{
		return ImGui::Checkbox(item.Label.c_str(), &item.IsChecked);
	}

	inline bool Combo(const std::string& label, int* currentItem, std::vector<const char*> items)
	{
		return ImGui::Combo(label.c_str(), currentItem, items.data(), static_cast<int>(items.size()));
	}

	inline bool CollapsingHeader(const std::string& label, ImGuiTreeNodeFlags flags = 0)
	{
		return ImGui::CollapsingHeader(label.c_str(), flags);
	}
}
