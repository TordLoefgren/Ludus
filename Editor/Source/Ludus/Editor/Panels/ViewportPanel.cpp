#include <pch.h>

#include <imgui/imgui.h>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/RenderView2D.h>
#include <Ludus/Engine/Math/Size.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Containers.h>

namespace Ludus::Editor::Panels
{
	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectSize(Ludus::Engine::Math::Size<int> framebufferSize)
	{
		auto availableSpace = ImGui::GetContentRegionAvail();

		const auto [width, height] = framebufferSize;
		const auto targetAspectRatio = static_cast<float>(width) / static_cast<float>(height);

		auto aspectWidth = availableSpace.x;
		auto aspectHeight = aspectWidth / targetAspectRatio;

		if (aspectHeight > availableSpace.y)
		{
			aspectHeight = availableSpace.y;
			aspectWidth = aspectHeight * targetAspectRatio;
		}

		return { aspectWidth, aspectHeight };
	}

	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectOffset(Ludus::Engine::Math::Vector2D aspectSize)
	{
		const auto availableSpace = ImGui::GetContentRegionAvail();
		const auto offsetX = (availableSpace.x - aspectSize.X) * 0.5f;
		const auto offsetY = (availableSpace.y - aspectSize.Y) * 0.5f;

		return { offsetX, offsetY };
	}

	ViewportPanel::ViewportPanel(std::string title, std::shared_ptr<Ludus::Engine::Graphics::Camera2D> camera)
		: m_Title(title), m_Camera(camera ? std::move(camera) : std::make_shared<Ludus::Engine::Graphics::Camera2D>()), m_Target(nullptr), m_PreviousTargetSize()
	{ }

	void ViewportPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		ImGuiWindowFlags flags = Ludus::Editor::Core::Constants::PanelFlags
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse;

		auto [r, g, b, a] = Ludus::Engine::Graphics::Colors::Black;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(r, g, b, a));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		auto windowTitle = CreateUniqueWindowTitle(m_Title);
		if (Ludus::UI::Containers::Window window(windowTitle.c_str(), &m_Open, flags); window)
		{
			const auto framebufferSize = context.SystemContext.Window.GetFramebufferSize();
			const auto aspectSize = GetViewportAspectSize(framebufferSize);

			const auto desiredSize = Ludus::Engine::Math::Size<int>(
				static_cast<int>(aspectSize.X),
				static_cast<int>(aspectSize.Y)
			);

			if (desiredSize.Width <= 0 || desiredSize.Height <= 0)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

				return;
			}

			if (!m_Target)
			{
				m_Target = std::make_shared<Ludus::Engine::Graphics::RenderTarget>(m_Title, desiredSize.Width, desiredSize.Height);
				m_PreviousTargetSize = desiredSize;
			}

			const auto aspectOffset = GetViewportAspectOffset(aspectSize);
			const auto cursor = ImGui::GetCursorPos();

			ImGui::SetCursorPos({ cursor.x + aspectOffset.X, cursor.y + aspectOffset.Y });

			if (desiredSize.Width != m_PreviousTargetSize.Width || desiredSize.Height != m_PreviousTargetSize.Height)
			{
				m_Target->Framebuffer.Resize(desiredSize.Width, desiredSize.Height);
				m_PreviousTargetSize = desiredSize;
			}

			ImGui::Image(
				(ImTextureID)(intptr_t)m_Target->ColorTexture.Handle(),
				{ aspectSize.X, aspectSize.Y },
				{ 0.0f, 1.0f },
				{ 1.0f, 0.0f }
			);

			m_Camera->SetViewport(desiredSize.Width, desiredSize.Height);

			// Register the render view with the render view registry, which is used by the rendering system every frame.
			Ludus::Engine::Graphics::RenderView2D renderView {
				.Camera = *m_Camera.get(),
				.Target = m_Target,
				.ViewportRect = Ludus::Engine::Math::Rect
				{
					{ cursor.x + aspectOffset.X, cursor.y + aspectOffset.Y },
					{ aspectSize.X, aspectSize.Y }
				}
			};

			context.SystemContext.RenderViews.Register(renderView);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}
