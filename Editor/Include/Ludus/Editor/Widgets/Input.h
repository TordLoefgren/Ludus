#pragma once

#include <Ludus/Editor/Core/Axis.h>

namespace Ludus::Editor::Widgets
{
	bool DragFloatAxisButton(const char* label, float* value, Ludus::Editor::Core::Axis axis, float speed = 0.1f);
}
