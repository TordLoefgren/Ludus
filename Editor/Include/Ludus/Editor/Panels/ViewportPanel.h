#pragma once

#include <string>

#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Panels/Constants.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Containers.h>

namespace Ludus::Editor::Panels
{
	class ViewportPanel final : public Ludus::Editor::Core::PanelSystem
	{
	private:
		std::string m_Title;

		Ludus::Engine::Math::Vector2D GetViewportAspectSize();
		Ludus::Engine::Math::Vector2D GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize);

	public:
		ViewportPanel(std::string title = "Viewport");

		virtual void DrawPanel() override;
	};
}
