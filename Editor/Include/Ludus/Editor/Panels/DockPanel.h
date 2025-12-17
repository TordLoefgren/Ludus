#pragma once

#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>

namespace Ludus::Editor::Panels
{
	class DockPanel final : public Ludus::Editor::Panels::IPanel
	{
	public:
		virtual void UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
