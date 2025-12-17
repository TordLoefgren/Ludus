#pragma once

#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/UI/Backend/ImGuiBackend.h>

namespace Ludus::UI::Systems
{
	class ImGuiSystem final : public Ludus::Engine::Core::ISystem
	{
	private:
		Ludus::UI::Backend::ImGuiBackend m_Backend { };

	public:
		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
		virtual void RenderImpl() override;
	};
}
