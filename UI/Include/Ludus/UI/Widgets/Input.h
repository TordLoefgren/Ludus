#pragma once

#include <cstdint>
#include <string>

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Widgets
{
	bool DragInt(const std::string& label, int* value, float speed = 1.0f);

	bool DragFloat(const std::string& label, float* value, float speed = 1.0f);

	bool DragFloatLabelButton(const char* label, float* value, float speed = 0.1f);

	bool InputInt(const std::string& label, int* value, int step = 0, int step_fast = 0, Ludus::UI::Flags::InputText flags = Ludus::UI::Flags::InputText::None);

	bool InputUInt8(const char* label, uint8_t* value);

	bool InputFloat(const std::string& label, float* value, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.2f", Ludus::UI::Flags::InputText flags = Ludus::UI::Flags::InputText::None);

	bool InputText(const std::string& label, std::string& text, Ludus::UI::Flags::InputText flags = Ludus::UI::Flags::InputText::None);
}
