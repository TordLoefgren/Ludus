#pragma once

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>

namespace Ludus::Editor::Panels
{
	class ImGuiDemoPanel final : public Ludus::Editor::Panels::IPanel
	{
	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Core::ProjectSessionContext& context) override { return &context.Shell.State.ActivePanelState.ShowImGuiDemoPanel; }

		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;
	};
}
