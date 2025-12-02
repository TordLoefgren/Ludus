#pragma once

#include <Ludus/UI/UIContext.h>

#include <Ludus/Core/ISystem.h>

namespace Ludus::Editor::Core
{
	class ImGuiSystem final : public Ludus::Core::ISystem
	{
	private:
		Ludus::UI::UIContext m_Context { };

	public:
		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
		virtual void RenderImpl() override;
	};
}
