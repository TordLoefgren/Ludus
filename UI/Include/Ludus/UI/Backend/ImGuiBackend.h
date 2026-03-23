#pragma once

struct GLFWwindow;

namespace Ludus::UI::Backend
{
	struct ImGuiBackend
	{
	public:
		void Initialize(GLFWwindow* window);
		void Shutdown();

		void Begin();
		void End();
	};
}
