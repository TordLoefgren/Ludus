#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Core/ViewportDisplayMode.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Engine/Core/Id.h>
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
		std::optional<Ludus::Engine::Graphics::RenderTarget> m_Target;
		Ludus::Engine::Math::Size<int> m_PreviousTargetSize;
		Ludus::Editor::Core::ViewportDisplayMode m_DisplayMode;
		std::optional<Ludus::Engine::Core::SceneId> m_SelectedSceneId = std::nullopt;

		bool m_IsCameraPanning = false;
		bool m_FollowActiveScene = true;

		Ludus::Engine::Math::Vector2D GetViewportAspectSize(float targetAspectRatio);
		Ludus::Engine::Math::Vector2D GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize);

		float ResolveTargetAspectRatio(const Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentationSettings) const;
		Ludus::Engine::Math::Size<int> ResolveRenderTargetSize(
			Ludus::Editor::Core::ProjectSessionContext& context,
			const Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentationSettings,
			Ludus::Engine::Math::Vector2D viewportDisplaySize
		) const;

		void HandleInput(Ludus::Editor::Core::ProjectSessionContext& context);

	public:
		ViewportPanel(
			std::string_view title = "Viewport",
			Ludus::Editor::Core::ViewportDisplayMode displayMode = Ludus::Editor::Core::ViewportDisplayMode::Editor
		);

		virtual Ludus::Editor::Panels::PanelKind GetPanelKind() const override { return Ludus::Editor::Panels::PanelKind::Viewport; }
		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;
	};
}
