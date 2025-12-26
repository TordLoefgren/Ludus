#include "pch.h"

#include <Ludus/UI/Backend/Fonts.h>
#include <Ludus/UI/Backend/ImGuiBackend.h>

namespace Ludus::UI::Backend
{
	void ImGuiBackend::Initialize(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		const auto sizePixels = 16.0f;
		const auto glyphMinAdvanceX = 16.0f;

		Ludus::UI::Backend::AddFont(io, "Resources/Fonts/ARIAL.TTF", sizePixels);
		Ludus::UI::Backend::AddMergedIconFont(io, "Resources/Fonts/FONT_AWESOME.TFF", sizePixels, glyphMinAdvanceX);

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();
	}

	void ImGuiBackend::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiBackend::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiBackend::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
