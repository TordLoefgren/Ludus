#include "pch.h"

#include <Ludus/Editor/Widgets/Input.h>

#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Widgets/Input.h>

namespace Ludus::Editor::Widgets
{
	namespace
	{
		const Ludus::Engine::Graphics::Color& GetAxisColor(const Ludus::Editor::Core::Axis axis)
		{
			switch (axis)
			{
				case Ludus::Editor::Core::Axis::X: return Ludus::UI::Context::ThemeContext::AxisX();
				case Ludus::Editor::Core::Axis::Y: return Ludus::UI::Context::ThemeContext::AxisY();
				case Ludus::Editor::Core::Axis::Z: return Ludus::UI::Context::ThemeContext::AxisZ();
				default:                           return Ludus::UI::Context::ThemeContext::TextPrimary();
			}
		}
	}

	bool DragFloatAxisButton(const char* label, float* value, const Ludus::Editor::Core::Axis axis, const float speed)
	{
		return Ludus::UI::Widgets::DragFloatColoredButton(label, value, GetAxisColor(axis), speed);
	}
}
