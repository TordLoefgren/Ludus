#pragma once

#include <Ludus/Editor/Core/PanelSystem.h>

namespace Ludus::Editor::Panels
{
	class ImGuiDemoPanel final : public Ludus::Editor::Core::PanelSystem
	{
	public:
		virtual void DrawPanel() override;
	};
}
