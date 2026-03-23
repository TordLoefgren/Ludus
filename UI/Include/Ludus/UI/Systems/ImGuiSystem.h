#pragma once

#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/UI/Backend/ImGuiBackend.h>

struct GLFWwindow;

namespace Ludus::UI::Systems
{
	class ImGuiSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		GLFWwindow* m_Window;
		Ludus::UI::Backend::ImGuiBackend m_Backend;

	public:
		ImGuiSystem(GLFWwindow* window);

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void BeginFrameImpl() override;
		virtual void EndFrameImpl() override;
	};
}
