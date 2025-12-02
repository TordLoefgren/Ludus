#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Ludus::UI
{
	inline bool s_IsBackendInitialized = false;

	class UIContext
	{
	private:
		GLFWwindow* m_Window = nullptr;

	public:
		void Initialize(GLFWwindow* window);
		void Shutdown();

		void Begin();
		void End();
	};
}
