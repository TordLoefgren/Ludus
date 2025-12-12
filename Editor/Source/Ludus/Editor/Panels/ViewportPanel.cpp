#include <pch.h>

#include <imgui/imgui.h>

#include <Ludus/Editor/Panels/ViewportPanel.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Editor::Panels
{
	Ludus::Engine::Math::Vector2D ViewportPanel::GetViewportAspectSize()
	{
		auto availableSpace = ImGui::GetContentRegionAvail();

		const auto [width, height] = m_SystemContext->Window.GetFramebufferSize();
		const auto targetAspectRatio = (float)width / height;

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
		: m_Title(title), m_Camera(camera ? std::move(camera) : std::make_shared<Ludus::Engine::Graphics::Camera2D>()), m_Target(nullptr)
	{ }

	void ViewportPanel::OnAttachImpl()
	{
		auto [width, height] = m_SystemContext->Window.GetFramebufferSize();
		m_Target = std::make_shared<Ludus::Engine::Graphics::RenderTarget>(m_Title, width, height);
	}

	void ViewportPanel::DrawPanel()
	{
		ImGuiWindowFlags flags = Constants::PanelFlags
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse;

		auto [r, g, b, a] = Ludus::Engine::Graphics::Colors::DarkGray;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(r, g, b, a));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		static bool open = true;
		if (Ludus::UI::Containers::Window window(m_Title.c_str(), &open, flags); window)
		{
			const auto aspectSize = GetViewportAspectSize();
			const auto aspectOffset = GetViewportAspectOffset(aspectSize);
			const auto cursor = ImGui::GetCursorPos();

			ImGui::SetCursorPos({ cursor.x + aspectOffset.X, cursor.y + aspectOffset.Y });

			m_Target->Framebuffer.Resize((int)aspectSize.X, (int)aspectSize.Y);
			m_Camera->SetViewport((int)aspectSize.X, (int)aspectSize.Y);

			ImGui::Image(
				(ImTextureID)(intptr_t)m_Target->ColorTexture.Handle(),
				{ aspectSize.X, aspectSize.Y },
				{ 0.0f, 1.0f },
				{ 1.0f, 0.0f }
			);

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

			m_SystemContext->RenderViews.Register(renderView);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}
