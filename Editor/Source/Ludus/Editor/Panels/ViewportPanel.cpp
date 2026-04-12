#include "pch.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/ViewportDisplayMode.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Engine/Windowing/Window.h>
#include <Ludus/UI/Context/ImageContext.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/SelectionContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ComboScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Panels
{
	ViewportPanel::ViewportPanel(
		std::string_view title,
		Ludus::Editor::Core::ViewportDisplayMode displayMode
	)
		: m_Title(CreateWindowTitleWithIcon(ICON_EYE, title)),
		m_Camera(),
		m_Target(std::nullopt),
		m_PreviousTargetSize(),
		m_DisplayMode(displayMode)
	{ }

	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectSize(float targetAspectRatio)
	{
		auto availableSpace = Ludus::UI::Context::WindowContext::GetContentRegionAvailable();

		if (availableSpace.X <= 0.0f || availableSpace.Y <= 0.0f || targetAspectRatio <= 0.0f)
		{
			return { 0.0f, 0.0f };
		}

		auto aspectWidth = availableSpace.X;
		auto aspectHeight = aspectWidth / targetAspectRatio;

		if (aspectHeight > availableSpace.Y)
		{
			aspectHeight = availableSpace.Y;
			aspectWidth = aspectHeight * targetAspectRatio;
		}

		return { aspectWidth, aspectHeight };
	}

	float ViewportPanel::ResolveTargetAspectRatio(const Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentationSettings) const
	{
		return renderPresentationSettings.GetAspectRatio();
	}

	Ludus::Engine::Math::Size<int> ViewportPanel::ResolveRenderTargetSize(
		Ludus::Editor::Core::ProjectSessionContext& context,
		const Ludus::Engine::Graphics::RenderPresentationSettings& renderPresentationSettings,
		Ludus::Engine::Math::Vector2D viewportDisplaySize
	) const
	{
		const auto fixedResolution = renderPresentationSettings.InternalResolution;
		if (renderPresentationSettings.UseFixedRenderResolution && fixedResolution.Width > 0 && fixedResolution.Height > 0)
		{
			return fixedResolution;
		}

		const auto [framebufferWidth, framebufferHeight] = context.HostContext.GetFramebufferSize();
		const auto [windowWidth, windowHeight] = context.HostContext.GetWindowSize();

		const auto scaleX = static_cast<float>(framebufferWidth) / static_cast<float>(std::max(1, windowWidth));
		const auto scaleY = static_cast<float>(framebufferHeight) / static_cast<float>(std::max(1, windowHeight));

		const auto desiredWidth = std::max(1, static_cast<int>(std::lround(viewportDisplaySize.X * scaleX)));
		const auto desiredHeight = std::max(1, static_cast<int>(std::lround(viewportDisplaySize.Y * scaleY)));

		return { desiredWidth, desiredHeight };
	}

	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize)
	{
		const auto availableSpace = Ludus::UI::Context::WindowContext::GetContentRegionAvailable();
		const auto offset = (availableSpace - aspectSize) * 0.5f;

		return offset;
	}

	void ViewportPanel::HandleInput(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto isMouseHovering = Ludus::UI::Context::InputContext::IsItemHovered();
		if (!isMouseHovering && !m_IsCameraPanning)
		{
			return;
		}

		auto& input = context.HostContext.GetInput();

		// Failsafe in the event that the mouse button up event is not caught.
		const auto rightMouseButtonPressed = input.GetMouseButton(Ludus::Engine::Windowing::MouseButton::Right);
		if (m_IsCameraPanning && !rightMouseButtonPressed)
		{
			m_IsCameraPanning = false;
		}

		if (isMouseHovering && input.GetMouseButtonDown(Ludus::Engine::Windowing::MouseButton::Right))
		{
			m_IsCameraPanning = true;
		}

		if (m_IsCameraPanning && input.GetMouseButtonUp(Ludus::Engine::Windowing::MouseButton::Right))
		{
			m_IsCameraPanning = false;
		}

		if (m_IsCameraPanning)
		{
			const auto viewportSize = m_Camera.GetViewportSize();
			if (viewportSize.X <= 0.0f || viewportSize.Y <= 0.0f)
			{
				return;
			}

			const auto worldSize = m_Camera.GetWorldRect().Size;

			const auto worldUnitPerPixelX = worldSize.X / viewportSize.X;
			const auto worldUnitPerPixelY = worldSize.Y / viewportSize.Y;

			const auto mouseDelta = input.GetMouseDelta();
			const auto worldDeltaX = mouseDelta.X * worldUnitPerPixelX;
			const auto worldDeltaY = mouseDelta.Y * -worldUnitPerPixelY;

			auto currentPosition = m_Camera.GetPosition();
			currentPosition.X += worldDeltaX;
			currentPosition.Y += worldDeltaY;

			m_Camera.SetPosition(currentPosition);
		}

		if (isMouseHovering)
		{
			const auto scrollY = input.GetMouseScrollOffset().Y;
			if (scrollY != 0.0f)
			{
				const auto currentSize = m_Camera.GetOrthographicSize();

				const auto factor = (scrollY > 0.0f) ? (1.0f - m_ZoomFactor) : (1.0f + m_ZoomFactor);
				const auto nextSize = std::clamp(currentSize * factor, m_MinZoom, m_MaxZoom);

				m_Camera.SetOrthographicSize(nextSize);
			}
		}
	}

	bool ViewportPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags
			| Ludus::UI::Flags::Window::NoScrollbar
			| Ludus::UI::Flags::Window::NoScrollWithMouse;

		Ludus::UI::Scope::StyleVarScope styleVar({ Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::WindowPadding, { 0.0f, 0.0f }) });

		auto windowTitle = CreateUniqueWindowTitle(m_Title);
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			const auto availableWidth = Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X;
			Ludus::UI::Context::LayoutContext::SetNextItemWidth(availableWidth);

			auto displayModeLabel = Ludus::UI::CreateLabel("Display Mode", "DisplayMode_Combo");
			auto _ = Ludus::UI::Widgets::ComboEnum(displayModeLabel.c_str(), m_DisplayMode);

			auto& registry = context.ProjectSession.RuntimeState.GetActiveSceneRegistry();

			auto activeSceneId = context.ProjectSession.GetPresentedSceneId();
			if (!m_SelectedSceneId || !registry.Contains(*m_SelectedSceneId))
			{
				m_SelectedSceneId = activeSceneId;
			}

			const auto& renderPresentationSettings = context.ProjectSession.RuntimeState.GetActiveRenderPresentationSettings();
			const auto targetAspectRatio = ResolveTargetAspectRatio(renderPresentationSettings);
			const auto aspectSize = GetViewportAspectSize(targetAspectRatio);
			const auto desiredSize = ResolveRenderTargetSize(context, renderPresentationSettings, aspectSize);

			if (desiredSize.Width <= 0 || desiredSize.Height <= 0)
			{
				return true;
			}

			if (!m_Target)
			{
				m_Target = Ludus::Engine::Graphics::RenderTarget::Create(m_Title, desiredSize.Width, desiredSize.Height);
				m_PreviousTargetSize = desiredSize;
			}

			if (desiredSize.Width != m_PreviousTargetSize.Width || desiredSize.Height != m_PreviousTargetSize.Height)
			{
				m_Target->Framebuffer.Resize(desiredSize.Width, desiredSize.Height);
				m_PreviousTargetSize = desiredSize;
			}

			const auto aspectOffset = GetViewportAspectOffset(aspectSize);
			const auto cursor = Ludus::UI::Context::WindowContext::GetCursorPosition();
			const auto viewportPosition = Ludus::Engine::Math::Vector2D(cursor.X + aspectOffset.X, cursor.Y + aspectOffset.Y);

			Ludus::UI::Context::WindowContext::SetCursorPosition(viewportPosition);
			m_Camera.SetViewport(desiredSize.Width, desiredSize.Height);

			Ludus::UI::Context::ImageContext::CreateImage(m_Target->ColorTexture.Handle(), aspectSize);

			std::optional<Ludus::Engine::Graphics::Camera2D> camera;

			if (m_DisplayMode == Ludus::Editor::Core::ViewportDisplayMode::Editor)
			{
				HandleInput(context);
				camera = m_Camera;
			}

			Ludus::Engine::Graphics::RenderViewRequest2D renderViewRequest {
				.Camera = camera,
				.SceneId = m_SelectedSceneId,
				.Target = &*m_Target,
				.ViewportRect = Ludus::Engine::Math::Rect::Create(viewportPosition, aspectSize)
			};

			context.ProjectSession.RuntimeState.GetActiveRenderViewRequestRegistry().Register(renderViewRequest);
		}

		return m_Open;
	}
}
