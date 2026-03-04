#pragma once

#include <memory>
#include <optional>
#include <string>

#include <Ludus/Editor/Core/ViewportDisplayMode.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/RenderPresentationSettings.h>
#include <Ludus/Engine/Graphics/RenderTarget.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Editor::Panels
{
	class ViewportPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		const float m_ZoomFactor = 0.1f;
		const float m_MinZoom = 0.1f;
		const float m_MaxZoom = 500.0f;

		std::string m_Title;
		Ludus::Engine::Graphics::Camera2D m_Camera;
		std::shared_ptr<Ludus::Engine::Graphics::RenderTarget> m_Target;
		Ludus::Engine::Math::Size<int> m_PreviousTargetSize;
		Ludus::Editor::Core::ViewportDisplayMode m_DisplayMode;
		std::optional<Ludus::Engine::Core::SceneHandle> m_SelectedSceneHandle = std::nullopt;

		bool m_IsCameraPanning = false;
		bool m_FollowActiveScene = true;

		Ludus::Engine::Math::Vector2D GetViewportAspectSize(float targetAspectRatio);
		Ludus::Engine::Math::Vector2D GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize);

		float ResolveTargetAspectRatio(const Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentationSettings) const;
		Ludus::Engine::Math::Size<int> ResolveRenderTargetSize(
			Ludus::Editor::Panels::PanelContext& context,
			const Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentationSettings,
			Ludus::Engine::Math::Vector2D viewportDisplaySize
		) const;

		void HandleInput(Ludus::Editor::Panels::PanelContext& context);

	public:
		ViewportPanel(
			std::string title = "Viewport",
			Ludus::Editor::Core::ViewportDisplayMode displayMode = Ludus::Editor::Core::ViewportDisplayMode::Editor
		);

		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
