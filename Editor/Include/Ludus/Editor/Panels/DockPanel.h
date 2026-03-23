#pragma once

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/UI/Labels.h>

namespace Ludus::Editor::Panels
{
	class DockPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		void DrawMenuBar(Ludus::Editor::Core::ProjectSessionContext& context);
		void DrawToolBar(Ludus::Editor::Core::ProjectSessionContext& context);

	public:
		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;
	};
}
