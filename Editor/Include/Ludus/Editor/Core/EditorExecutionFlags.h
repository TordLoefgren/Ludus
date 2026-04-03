#pragma once

#include <cstdint>

#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Mask.h>

namespace Ludus::Editor::Core
{
	enum class EditorExecutionFlags : uint32_t
	{
		None = 0,
		EditorToolsEnabled = 1 << 16,
		ImGuiDemoEnabled = 1 << 17,
	};

	constexpr uint32_t DefaultEditorExecutionMask =
		Ludus::Engine::Core::Mask(EditorExecutionFlags::None) |
		Ludus::Engine::Core::Mask(EditorExecutionFlags::EditorToolsEnabled) |
		Ludus::Engine::Core::Mask(Ludus::Engine::Core::ExecutionFlags::RenderingEnabled);
}
