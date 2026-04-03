#include "pch.h"

#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Panels/ImGuiDemoPanel.h>
#include <Ludus/UI/Widgets/Demo.h>

namespace Ludus::Editor::Panels
{
	bool ImGuiDemoPanel::IsAvailable(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		return context.Shell.State.Mode == Ludus::Editor::Core::EditorMode::Session &&
			context.HostContext.GetExecutionFlags().HasAny(Ludus::Editor::Core::EditorExecutionFlags::ImGuiDemoEnabled);
	}

	bool ImGuiDemoPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		Ludus::UI::Widgets::ShowDemoWindow(&m_Open);

		return true;
	}
}
