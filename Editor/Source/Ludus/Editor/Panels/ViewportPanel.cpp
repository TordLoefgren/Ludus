#include <pch.h>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/ImageContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>

namespace Ludus::Editor::Panels
{
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

	ViewportPanel::ViewportPanel(std::string title, std::shared_ptr<Ludus::Engine::Graphics::Camera2D> camera)
		: m_Title(title), m_Camera(camera ? std::move(camera) : std::make_shared<Ludus::Engine::Graphics::Camera2D>()), m_Target(nullptr), m_PreviousTargetSize()
	{ }

	void ViewportPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto flags = Ludus::Editor::Core::Constants::PanelFlags
			| Ludus::UI::Flags::Window::NoScrollbar
			| Ludus::UI::Flags::Window::NoScrollWithMouse;

		Ludus::UI::Scope::StyleColorScope styleColor({ Ludus::UI::Scope::StyleColor(Ludus::UI::Scope::Color::ImGuiCol_WindowBg, Ludus::Engine::Graphics::Colors::Black) });
		Ludus::UI::Scope::StyleVarScope styleVar({ Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::WindowPadding, {0.0f, 0.0f }) });

		auto windowTitle = CreateUniqueWindowTitle(m_Title);
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, flags); window)
		{
			const auto framebufferSize = context.SystemContext.Window.GetFramebufferSize();
			const auto aspectSize = GetViewportAspectSize(framebufferSize);

			const auto desiredSize = Ludus::Engine::Math::Size<int>(
				static_cast<int>(aspectSize.X),
				static_cast<int>(aspectSize.Y)
			);

			if (desiredSize.Width <= 0 || desiredSize.Height <= 0)
			{
				return;
			}

			if (!m_Target)
			{
				m_Target = std::make_shared<Ludus::Engine::Graphics::RenderTarget>(m_Title, desiredSize.Width, desiredSize.Height);
				m_PreviousTargetSize = desiredSize;
			}

			const auto aspectOffset = GetViewportAspectOffset(aspectSize);
			const auto cursor = Ludus::UI::Context::WindowContext::GetCursorPosition();
			const auto viewportSize = Ludus::Engine::Math::Vector2D(cursor.X + aspectOffset.X, cursor.Y + aspectOffset.Y);

			Ludus::UI::Context::WindowContext::SetCursorPosition(viewportSize);

			if (desiredSize.Width != m_PreviousTargetSize.Width || desiredSize.Height != m_PreviousTargetSize.Height)
			{
				m_Target->Framebuffer.Resize(desiredSize.Width, desiredSize.Height);
				m_PreviousTargetSize = desiredSize;
			}

			Ludus::UI::Context::ImageContext::CreateImage(m_Target->ColorTexture.Handle(), aspectSize);

			m_Camera->SetViewport(desiredSize.Width, desiredSize.Height);

			// Register the render view with the render view registry, which is used by the rendering system every frame.
			Ludus::Engine::Graphics::RenderView2D renderView {
				.Camera = *m_Camera.get(),
				.Target = m_Target,
				.ViewportRect = Ludus::Engine::Math::Rect
				{
					viewportSize,
					aspectSize
				}
			};

			context.SystemContext.RenderViews.Register(renderView);
		}
		else
		{
			LUDUS_LOG_INFO("Viewport window body NOT executing");
		}
	}
}
