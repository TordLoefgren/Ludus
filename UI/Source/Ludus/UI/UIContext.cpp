#include <Ludus/UI/UIContext.h>

namespace Ludus::UI
{
	void UIContext::Initialize(GLFWwindow* window)
	{
		m_Window = window;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		if (!s_IsBackendInitialized)
		{
			ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
			ImGui_ImplOpenGL3_Init();

			s_IsBackendInitialized = true;
		}
	}

	void UIContext::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UIContext::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UIContext::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
