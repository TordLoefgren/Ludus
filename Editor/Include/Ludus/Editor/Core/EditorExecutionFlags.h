#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/Enums/EnumBits.h>
#include <Ludus/Engine/Core/ExecutionFlags.h>

namespace Ludus::Editor::Core
{
	enum class EditorExecutionFlags : uint32_t
	{
		None = 0,
		EditorToolsEnabled = 1 << 16,
		ImGuiDemoEnabled = 1 << 17,
	};

	constexpr uint32_t DefaultEditorExecutionMask =
		Ludus::Engine::Core::Enums::ToUnderlyingType(EditorExecutionFlags::None) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(EditorExecutionFlags::EditorToolsEnabled) |
		Ludus::Engine::Core::Enums::ToUnderlyingType(Ludus::Engine::Core::ExecutionFlags::RenderingEnabled);
}
