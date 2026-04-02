#pragma once

#include <Ludus/Editor/Core/Axis.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Editor::Widgets
{
	bool DragFloatAxisButton(
		const char* label,
		float* value,
		Ludus::Editor::Core::Axis axis,
		const Ludus::Engine::Math::Vector2D& size = { },
		float speed = 0.1f
	);

	bool DrawAxisFloatField(const char* id, float* value, const Ludus::Editor::Core::Axis axis);

	bool DrawVector2Field(const char* id, float* x, float* y);
}
