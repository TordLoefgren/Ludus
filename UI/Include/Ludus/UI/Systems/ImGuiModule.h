#pragma once

#include <memory>

#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPhaseOrder.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/UI/Systems/ImGuiSystem.h>

namespace Ludus::UI::Systems
{
	static bool s_IsImGuiEnabled = false;

	inline void RegisterImGui(Ludus::Engine::Core::ApplicationBuilder& builder)
	{
		if (!s_IsImGuiEnabled)
		{
			builder.Configure(
				[](Ludus::Engine::Core::Application& application)
				{
					auto imGuiSystem = std::make_unique<Ludus::UI::Systems::ImGuiSystem>();

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
}
