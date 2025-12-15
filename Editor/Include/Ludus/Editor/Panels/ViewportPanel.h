#pragma once

#include <memory>
#include <string>

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Editor::Panels
{
	class ViewportPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		std::string m_Title;
		std::shared_ptr<Ludus::Engine::Graphics::Camera2D> m_Camera;
		std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> m_Target;
		Ludus::Engine::Math::Size<int> m_PreviousTargetSize;

		Ludus::Engine::Math::Vector2D GetViewportAspectSize(Ludus::Engine::Math::Size<int> framebufferSize);
		Ludus::Engine::Math::Vector2D GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize);

	public:
		ViewportPanel(std::string title = "Viewport", std::shared_ptr<Ludus::Engine::Graphics::Camera2D> camera = nullptr);

		virtual void UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
