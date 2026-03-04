#pragma once

#include <Ludus/Editor/Core/EditorMode.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>

namespace Ludus::Editor::Panels
{
	class WelcomePanel final : public Ludus::Editor::Panels::IPanel
	{
	public:
		virtual bool IsAvailable(Ludus::Editor::Panels::PanelContext& context) override
		{
			return context.EditorContext.State.Mode == Ludus::Editor::Core::EditorMode::Startup;
		}

		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
