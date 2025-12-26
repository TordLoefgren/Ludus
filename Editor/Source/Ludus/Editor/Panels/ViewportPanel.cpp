#include <pch.h>

#include <algorithm>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/Utilities.h>
#include <Ludus/Editor/Core/ViewportDisplayMode.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/ImageContext.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/SelectionContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ComboScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Panels
{
	ViewportPanel::ViewportPanel(std::string title, Ludus::Editor::Core::ViewportDisplayMode displayMode)
		: m_Title(title), m_Camera(), m_Target(nullptr), m_PreviousTargetSize(), m_DisplayMode(displayMode)
	{ }

	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectSize(Ludus::Engine::Math::Size<int> framebufferSize)
	{
		auto availableSpace = Ludus::UI::Context::WindowContext::GetContentRegionAvailable();

		const auto [width, height] = framebufferSize;
		const auto targetAspectRatio = static_cast<float>(width) / static_cast<float>(height);

		auto aspectWidth = availableSpace.X;
		auto aspectHeight = aspectWidth / targetAspectRatio;

		if (aspectHeight > availableSpace.Y)
		{
			aspectHeight = availableSpace.Y;
			aspectWidth = aspectHeight * targetAspectRatio;
		}

		return { aspectWidth, aspectHeight };
	}

	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize)
	{
		const auto availableSpace = Ludus::UI::Context::WindowContext::GetContentRegionAvailable();
		const auto offset = (availableSpace - aspectSize) * 0.5f;

		return offset;
	}

	void ViewportPanel::HandleInput(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto isMouseHovering = Ludus::UI::Context::InputContext::IsItemHovered();
		if (!isMouseHovering && !m_IsCameraPanning)
		{
			return;
		}

		auto& input = context.SystemContext.Input;

		// Failsafe in the event that the mouse button up event is not caught.
		const auto rightMouseButtonPressed = input.GetMouseButton(Ludus::Engine::Platform::MouseButton::Right);
		if (m_IsCameraPanning && !rightMouseButtonPressed)
		{
			m_IsCameraPanning = false;
		}

		if (isMouseHovering && input.GetMouseButtonDown(Ludus::Engine::Platform::MouseButton::Right))
		{
			m_IsCameraPanning = true;
		}

		if (m_IsCameraPanning && input.GetMouseButtonUp(Ludus::Engine::Platform::MouseButton::Right))
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
			const auto worldDeltaX = mouseDelta.X * -worldUnitPerPixelX;
			const auto worldDeltaY = mouseDelta.Y * worldUnitPerPixelY;

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
				const auto factor = (scrollY > 0.0f) ? (1.0f - m_ZoomFactor) : (1.0f + m_ZoomFactor);
				const auto currentSize = m_Camera.GetOrthographicSize();
				const auto nextSize = std::clamp(currentSize * factor, m_MinZoom, m_MaxZoom);
				m_Camera.SetOrthographicSize(nextSize);
			}
		}
	}

	bool ViewportPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags
			| Ludus::UI::Flags::Window::NoScrollbar
			| Ludus::UI::Flags::Window::NoScrollWithMouse;

		Ludus::UI::Scope::StyleVarScope styleVar({ Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::WindowPadding, {0.0f, 0.0f }) });

		auto windowTitle = CreateUniqueWindowTitle(m_Title);
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##Mode_Combo", m_DisplayMode);

			auto& sceneManager = context.SystemContext.SceneManager;
			auto active = sceneManager.GetActiveSceneHandle();

			if (!m_SelectedSceneHandle.has_value() || !sceneManager.Contains(m_SelectedSceneHandle.value()))
			{
				m_SelectedSceneHandle = active;
			}

			auto scenes = sceneManager.View();
			const auto comboLabel = Ludus::UI::CreateLabel("", "Scenes");
			const auto preview = m_SelectedSceneHandle.has_value() ? std::format("Scene {}", m_SelectedSceneHandle.value()) : "None";

			Ludus::UI::Context::LayoutContext::SameLine();

			const auto availableWidth = Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X;
			Ludus::UI::Context::LayoutContext::SetNextItemWidth(availableWidth);

			if (Ludus::UI::Scope::ComboScope combo(comboLabel.c_str(), preview.c_str()); combo)
			{
				if (scenes.empty())
				{
					const auto _ = Ludus::UI::Widgets::Selectable("None", false);
				}
				else
				{
					for (const auto& scene : scenes)
					{
						const bool isSelected = m_SelectedSceneHandle == scene.Handle;
						const auto itemLabel = std::format("Scene {}", scene.Handle);
						if (Ludus::UI::Widgets::Selectable(itemLabel.c_str(), isSelected))
						{
							m_SelectedSceneHandle = scene.Handle;
						}

						if (isSelected)
						{
							Ludus::UI::Context::SelectionContext::SetItemDefaultFocus();
						}
					}
				}
			}

			const auto framebufferSize = context.SystemContext.Window.GetFramebufferSize();
			const auto aspectSize = GetViewportAspectSize(framebufferSize);

			const auto desiredSize = Ludus::Engine::Math::Size<int>(
				static_cast<int>(aspectSize.X),
				static_cast<int>(aspectSize.Y)
			);

			if (desiredSize.Width <= 0 || desiredSize.Height <= 0)
			{
				return true;
			}

			if (!m_Target)
			{
				m_Target = std::make_shared<Ludus::Engine::Graphics::RenderTarget>(m_Title, desiredSize.Width, desiredSize.Height);
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
				.SceneHandle = m_SelectedSceneHandle,
				.Target = m_Target,
				.ViewportRect = Ludus::Engine::Math::Rect
				{
					viewportPosition,
					aspectSize
				}
			};

			context.SystemContext.RenderViewRequests.Register(renderViewRequest);
		}

		return m_Open;
	}
}
