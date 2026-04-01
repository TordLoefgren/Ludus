#pragma once

#include <cstdint>

struct GLFWwindow;

namespace Ludus::UI::Backend
{
	struct ImGuiBackend
	{
	private:
		std::uint64_t m_CurrentThemeRevision = 0;

		void ApplyPendingTheme();

	public:
		void Initialize(GLFWwindow* window);
		void Shutdown();

		void Begin();
		void End();
	};
}
