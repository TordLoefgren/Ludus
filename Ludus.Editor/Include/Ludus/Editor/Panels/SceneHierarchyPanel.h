#pragma once

#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/UI/Containers.h>
#include <Ludus/UI/Widgets.h>

namespace Ludus::Editor::Panels
{
	class SceneHierarchyPanel final : public Ludus::Editor::Core::PanelSystem
	{
	public:
		virtual void DrawPanel() override;
	};
}
