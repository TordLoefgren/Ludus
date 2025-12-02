#pragma once

#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/UI/Utilities.h>

namespace Ludus::Editor::Panels
{
	class ImGuiDemoPanel final : public Ludus::Editor::Core::PanelSystem
	{
	public:
		virtual void DrawPanel() override;
	};
}
