#pragma once

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>

namespace Ludus::Editor::Panels
{
	class InspectorPanel final : public Ludus::Editor::Panels::IPanel
	{
	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Panels::PanelContext& context) override { return &context.ActivePanelState.ShowInspectorPanel; }

		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
