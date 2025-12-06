#include "Ludus/Editor/Panels/ViewportPanel.h"

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

	ViewportPanel::ViewportPanel(std::string title)
		: m_Title(title)
	{ }

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

			auto* targetPtr = m_SystemContext->Resources.Get<std::shared_ptr<Ludus::Engine::Graphics::RenderTarget>>().get();
			if (targetPtr)
			{
				targetPtr->Framebuffer.Resize((int)aspectSize.X, (int)aspectSize.Y);

				ImGui::Image(
					(ImTextureID)(intptr_t)targetPtr->ColorTexture.Handle(),
					{ aspectSize.X, aspectSize.Y },
					{ 0.0f, 1.0f },
					{ 1.0f, 0.0f }
				);
			}
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}
