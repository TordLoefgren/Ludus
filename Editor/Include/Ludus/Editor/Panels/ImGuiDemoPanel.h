#pragma once

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>

namespace Ludus::Editor::Panels
{
	class ImGuiDemoPanel final : public Ludus::Editor::Panels::IPanel
	{
	public:
		virtual Ludus::Editor::Panels::PanelKind GetPanelKind() const override { return Ludus::Editor::Panels::PanelKind::ImGuiDemo; }
		virtual bool UsesPanelState() const override { return true; }

		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;
	};
}
