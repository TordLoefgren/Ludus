#pragma once

#include <string>

#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/RenderTarget.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/UI/Containers.h>

namespace Ludus::Editor::Panels
{
	class ViewportPanel final : public Ludus::Editor::Core::PanelSystem
	{
	private:
		std::string m_Title;

		Ludus::Math::Vector2D GetViewportAspectSize();
		Ludus::Math::Vector2D GetViewportAspectOffset(Ludus::Math::Vector2D aspectSize);

	public:
		ViewportPanel(std::string title = "Viewport");

		virtual void DrawPanel() override;
	};
}
