#pragma once

#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/UI/Containers.h>				

namespace Ludus::Editor::Panels
{
	class DockPanel final : public Ludus::Editor::Core::PanelSystem
	{
	public:
		virtual void DrawPanel() override;
	};
}
