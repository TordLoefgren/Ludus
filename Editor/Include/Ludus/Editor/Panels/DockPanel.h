#pragma once

#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>

namespace Ludus::Editor::Panels
{
	class DockPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		bool m_IsPlayButtonPressed = false;
		bool m_IsPauseButtonPressed = false;

		void DrawMenuBar(Ludus::Editor::Panels::PanelContext& context);
		void DrawToolBar(Ludus::Editor::Panels::PanelContext& context);
	public:
		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
