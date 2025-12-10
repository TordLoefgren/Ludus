#pragma once

#include <imgui/imgui.h>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/UI/ImGuiSystem.h>

namespace Ludus::UI::Utilities
{
	inline bool s_IsImGuiEnabled = false;

	inline void UseImGui(Ludus::Engine::Core::ApplicationBuilder& builder)
	{
		if (!s_IsImGuiEnabled)
		{
			builder.Configure(
				[](Ludus::Engine::Core::Application& application)
				{
					auto imGuiSystem = std::make_unique<Ludus::UI::ImGuiSystem>();

					application.AddSystem(
						{
							{ Ludus::Engine::Core::SystemPhase::Update, nullptr, Ludus::Engine::Core::SystemPhaseOrder::Before },
							{ Ludus::Engine::Core::SystemPhase::Render, nullptr, Ludus::Engine::Core::SystemPhaseOrder::After }
						},
						std::move(imGuiSystem));
				}
			);

			s_IsImGuiEnabled = true;
		}
		else
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add ImGui system more than once.");
		}
	}

	inline void ShowDemoWindow() { ImGui::ShowDemoWindow(); }

	inline ImGuiIO& GetIO() { return ImGui::GetIO(); }
}
