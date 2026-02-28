#pragma once

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/UI/Labels.h>

namespace Ludus::Editor::Panels
{
	class DockPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		void DrawMenuBar(Ludus::Editor::Panels::PanelContext& context);
		void DrawToolBar(Ludus::Editor::Panels::PanelContext& context);

	public:
		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
