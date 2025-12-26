#pragma once

#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Engine/Core/SystemContext.h>

namespace Ludus::Editor::Panels
{
	struct PanelContext
	{
		Ludus::Engine::Core::SystemContext& SystemContext;
		Ludus::Editor::Core::EditorContext& EditorContext;
		float DeltaTime;
	};
}
