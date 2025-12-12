#pragma once

#include <Ludus/UI/UIContext.h>

#include <Ludus/Engine/Core/ISystem.h>

namespace Ludus::UI
{
	class ImGuiSystem final : public Ludus::Engine::Core::ISystem
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
